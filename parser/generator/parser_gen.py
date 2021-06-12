import argparse
import pathlib
import json
import logging
import datetime

JSON_EXPR_CLASSES_NAME: str = "expression_classes"
JSON_PRIMARY_INC_NAME: str = "primary_includes"
JSON_SECONDARY_INC_NAME: str = "secondary_includes"
JSON_NAMESPACE_NAME: str = "generated_namespace"
JSON_CLASS_BASE_NAME: str = "expression_class_bases"

TYPE_ENUM_NAME: str = "parser_class_type"


def type_value_of(name: str):
    return "PC_TYPE_{}".format(name)


def visitor_method_of(name: str) -> str:
    return "visit_{0}".format(name)


class BaseClass:
    __name: str
    __accessibility: str
    __crtp: bool = False

    def __init__(self, name: str, acc: str, crtp: bool):
        self.__name = name
        self.__accessibility = acc
        self.__crtp = crtp

    @property
    def crtp(self) -> bool:
        return self.__crtp

    @property
    def string(self):
        return "{} {}".format(self.__accessibility, self.__name)

    def string_crtp(self, crtp_param: str = ""):
        if not self.__crtp and len(crtp_param) != 0:
            raise RuntimeError("Not a CRTP base, but CRTP parameter is provided")

        return "{} {}<{}>".format(self.__accessibility, self.__name, crtp_param)


class IncludeHeader:
    __name: str

    def __init__(self, name: str):
        self.__name = name

    @property
    def line(self) -> str:
        return "#include <{}>".format(self.__name)


class ParserClassMemberDef:
    __type: str
    __name: str

    def __init__(self, _name: str, _type: str):
        self.__name = _name
        self.__type = _type

    @property
    def definition(self) -> str:
        return "{0} {1};".format(self.__type, self.__name)
        pass

    @property
    def getter(self):
        yield "[[nodiscard]] auto& get_{}(){{".format(self.__name[:-1])
        yield "return this->{};}}".format(self.__name)
        pass

    @property
    def setter(self):
        yield "void set_{0}(const {1}&{0}){{".format(self.__name[:-1], self.__type)
        yield "{}={};}}".format(self.__name, self.__name[:-1])


class ParserClassConstructorDef:
    __def: str

    def __init__(self, __name: str, members_: dict):
        self.__def = "[[nodiscard]] explicit {0}(".format(__name)

        for m in members_:
            self.__def += "{0} {1},".format(m["type"], str(m["name"])[:-1])

        self.__def = self.__def[:-1]

        self.__def += "):"

        for m in members_:
            self.__def += "{0}({1}),".format(m["name"], str(m["name"])[:-1])

        self.__def = self.__def[:-1]

        self.__def += "{"
        self.__def += "}"

    @property
    def definition(self) -> str:
        return self.__def


class ParserClass:
    __name: str = ""
    __members: list[ParserClassMemberDef] = list()
    __parents: list[BaseClass] = list()

    __ctor: ParserClassConstructorDef

    def __init__(self, _name: str, members_: dict, base):
        self.__name = _name
        self.__members = list(ParserClassMemberDef(m["name"], m["type"]) for m in members_)

        self.__parents = list(base)

        self.__ctor = ParserClassConstructorDef(_name, members_)

    def __concat_base_classes(self) -> str:
        ret: str = ''
        for c in self.__parents:
            if c.crtp:
                ret += (c.string_crtp(self.__name) + ",")
            else:
                ret += (c.string + ",")
        return ret[:-1]

    @property
    def name(self) -> str:
        return self.__name

    @property
    def lines(self):
        yield "class {0}:{1}{{".format(self.__name, self.__concat_base_classes())

        yield "public:"

        yield "{0}(const {0}&)=default;".format(self.__name)
        yield "{0}({0}&&)=default;".format(self.__name)
        yield "{0}& operator=(const {0}&)=default;".format(self.__name)

        yield self.__ctor.definition

        for m in self.__members:
            for s in m.getter:
                yield s

        for m in self.__members:
            for s in m.setter:
                yield s

        yield "[[nodiscard]] {0} get_type()const override{{return {1};}}".format(TYPE_ENUM_NAME,
                                                                                 type_value_of(self.name))

        yield "template<typename T>[[nodiscard]] T accept(visitor<T> &vis){"
        yield "return vis.{}(this);".format(visitor_method_of(self.name))
        yield "}"

        yield "private:"

        for m in self.__members:
            yield m.definition

        yield "};"


class Visitor:
    __classes: list[ParserClass]

    def __init__(self, classes):
        self.__classes = list(classes)

    @property
    def primary_lines(self):
        yield "template<typename T> class visitor{"
        yield "public:"

        for c in self.__classes:
            yield "virtual T {0}(class {1}*)=0;".format(visitor_method_of(c.name), c.name)

        yield "};"

    @property
    def secondary_lines(self):
        yield ""

    @property
    def accept_func(self):
        yield "template<typename T>static inline T accept(const expression& expr,visitor<T> &vis){"
        yield "switch(expr.get_type()){"

        for c in self.__classes:
            yield "case {}:".format(type_value_of(c.name))

            yield "return (( {0} &)(expr)).accept<T>(vis);".format(c.name)

            yield "break;"

        yield "default:"
        yield 'throw std::invalid_argument("{}");'.format("expr")

        yield "}"
        yield "}"


class ParserClassTypeEnum:
    __classes: list[str]

    def __init__(self, classes):
        self.__classes = list(classes)

    @property
    def lines(self):
        yield "enum {}{{".format(TYPE_ENUM_NAME)

        yield "{},".format(type_value_of("invalid"))

        for c in self.__classes:
            yield "{},".format(type_value_of(c))

        yield "};"


class Namespace:
    __name: str
    __classes: list[ParserClass]
    __visitor: Visitor
    __enum: ParserClassTypeEnum

    def __init__(self, name: str, classes):
        self.__name = name
        self.__classes = list(classes)
        self.__enum = ParserClassTypeEnum(list(c.name for c in self.__classes))
        self.__visitor = Visitor(self.__classes)

    @property
    def primary_lines(self) -> str:
        yield "namespace {} {{".format(self.__name)

        for l1 in self.__visitor.primary_lines:
            yield l1

        for c in self.__classes:
            for line in c.lines:
                yield line

        for ac in self.__visitor.accept_func:
            yield ac

        yield "}"

    @property
    def secondary_lines(self):
        yield "namespace {} {{".format(self.__name)

        for e in self.__enum.lines:
            yield e

        for l1 in self.__visitor.secondary_lines:
            yield l1

        yield "}"


def primary_include_files(args: argparse):
    with open(str(args.config[0]), "r") as conf:
        conf_dict: dict = json.load(conf)

        includings: dict = conf_dict[JSON_PRIMARY_INC_NAME]

        return (IncludeHeader(h) for h in includings)


def secondary_include_files(args: argparse):
    with open(str(args.config[0]), "r") as conf:
        conf_dict: dict = json.load(conf)

        includings: dict = conf_dict[JSON_SECONDARY_INC_NAME]

        return (IncludeHeader(h) for h in includings)


def expression_classes(args: argparse):
    with open(str(args.config[0]), "r") as conf:
        conf_dict: dict = json.load(conf)

        expr_classes: dict = conf_dict[JSON_EXPR_CLASSES_NAME]
        base_classes: dict = conf_dict[JSON_CLASS_BASE_NAME]

        return (ParserClass(expr_cls_def["name"], expr_cls_def["member"],
                            (BaseClass(b["name"], b["access"], b["crtp"]) for b in base_classes if
                             b["id"] in expr_cls_def["base"]))
                for expr_cls_def in
                expr_classes)


def root_namespace(args: argparse):
    with open(str(args.config[0]), "r") as conf:
        conf_dict: dict = json.load(conf)

        namespace_conf: str = conf_dict[JSON_NAMESPACE_NAME]

        expr_classes: dict = conf_dict[JSON_EXPR_CLASSES_NAME]

        return Namespace(namespace_conf, expression_classes(args))


def write_back(path: str, head: list, content: list, tail: list):
    logging.info("Writing result to {}.".format(path))

    with open(path, "w") as f:
        for h in head:
            f.write("{}\n".format(h))

        for c in content:
            f.write("{}\n".format(c))

        for t in tail:
            f.write("{}\n".format(t))


def generate(args: argparse):
    logging.info("Head template file is {}.".format(args.head[0]))
    logging.info("Tail template file is {}.".format(args.tail[0]))

    head: list = list()
    head.append("#pragma once")

    with open(args.head[0], "r") as template:
        line = template.readline()
        while len(line) != 0:
            head.append(line.strip())
            line = template.readline()

    head.append("#pragma once")

    tail: list = list()
    with open(args.tail[0], "r") as template:
        line = template.readline()
        while len(line) != 0:
            tail.append(line.strip())
            line = template.readline()

    primary_head = head.copy()

    for h in primary_include_files(args):
        primary_head.append(h.line)

    secondary_path: pathlib.Path = pathlib.Path((args.secondary[0]))
    primary_head.append('#include "{}"'.format(secondary_path.name))

    write_back(args.primary[0], primary_head, list(root_namespace(args).primary_lines), tail)

    secondary_head = head.copy()
    for h in secondary_include_files(args):
        secondary_head.append(h.line)
    write_back(args.secondary[0], secondary_head, list(root_namespace(args).secondary_lines), tail)


def main():
    def validate_config_file(arg: str) -> str:
        path: pathlib.Path = pathlib.Path(arg)

        if not path.is_file():
            raise argparse.ArgumentError("FATAL:{} should be a file".format(arg))

        if path.exists():
            return str(path.absolute().as_posix())
        else:
            raise argparse.ArgumentError("FATAL: path {} not exists".format(arg))

    def validate_target_file(arg: str) -> str:
        path: pathlib.Path = pathlib.Path(arg)

        return str(path.absolute().as_posix())

    arg_parser: argparse = argparse.ArgumentParser(description="Generator for parser classes")

    arg_parser.add_argument('-c', '--config', type=lambda x: validate_config_file(x), nargs=1,
                            help='configuration file', required=True)

    arg_parser.add_argument('-H', '--head', type=lambda x: validate_config_file(x), nargs=1,
                            help='head template file', required=True)

    arg_parser.add_argument('-t', '--tail', type=lambda x: validate_config_file(x), nargs=1,
                            help='tail template file', required=True)

    arg_parser.add_argument('-p', '--primary', type=lambda x: validate_target_file(x), nargs=1,
                            help='primary generated file', required=True)

    arg_parser.add_argument('-s', '--secondary', type=lambda x: validate_target_file(x), nargs=1,
                            help='secondary generated file', required=True)

    generate(arg_parser.parse_args())


if __name__ == "__main__":
    main()
