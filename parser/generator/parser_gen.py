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

use_shared_ptr: bool = True


def type_value_of(name: str):
    return "PC_TYPE_{}".format(name)


def visitor_method_of(name: str) -> str:
    return "visit_{0}".format(name)


class IncludeHeader:

    def __init__(self, name: str):
        self.__name = name

    @property
    def line(self) -> str:
        return "#include <{}>".format(self.__name)


class ParserClassMemberDef:

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

    def __this_pointer(self) -> str:
        if use_shared_ptr:
            return "shared_from_this()"
        else:
            return "this"

    def __visitable_base(self):
        for b in self.__parents:
            if b.visitable:
                return b

        return None

    def add_to_parent(self):
        for p in self.__parents:
            if p.visitable:
                p.add_super(self)
                return

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

        vb = self.__visitable_base()
        if vb is not None:
            yield "template<typename T>[[nodiscard]] T accept({}<T> &vis){{".format(vb.visitor_name)

            yield "return vis.{0}({1});".format(visitor_method_of(self.name), self.__this_pointer())
            yield "}"

        yield "private:"

        for m in self.__members:
            yield m.definition

        yield "};"


class Visitor:

    def __init__(self, name: str, base: str, classes):
        self.__name = name
        self.__base = base
        self.__classes = list(classes)

    def __pointer_type(self, name: str):
        if use_shared_ptr:
            return "const std::shared_ptr<class {}> &".format(name)
        else:
            return "class {}*".format(name)

    @property
    def primary_lines(self):
        yield "template<typename T> class {}{{".format(self.__name)
        yield "public:"

        for c in self.__classes:
            yield "virtual T {0}({1})=0;".format(visitor_method_of(c.name), self.__pointer_type(c.name))

        yield "};"

    @property
    def secondary_lines(self):
        yield ""

    @property
    def accept_func(self):
        yield "template<typename T>static inline T accept(const {}& visitee,{}<T> &vis){{".format(self.__base,
                                                                                                  self.__name)
        yield "switch(visitee.get_type()){"

        for c in self.__classes:
            yield "case {}:".format(type_value_of(c.name))

            yield "return (( {0} &)(visitee)).accept<T>(vis);".format(c.name)

            yield "break;"

        yield "default:"
        yield 'throw std::invalid_argument("{}");'.format("visitee")

        yield "}"
        yield "}"


class BaseClass:

    def __init__(self, name: str, acc: str, crtp: bool, visitable: bool, the_id: int):
        self.__name = name
        self.__accessibility = acc
        self.__crtp = crtp
        self.__visitable = visitable
        self.__id = the_id
        self.__super = list()

    def add_super(self, cls: ParserClass):
        self.__super.append(cls)

    def get_visitor(self) -> Visitor:
        if not self.__visitable:
            raise RuntimeError()
        return Visitor(self.visitor_name, self.__name, self.__super)

    @property
    def visitor_name(self):
        return "{}_visitor".format(self.__name)

    @property
    def id(self):
        return self.__id

    @property
    def crtp(self) -> bool:
        return self.__crtp

    @property
    def string(self):
        return "{} {}".format(self.__accessibility, self.__name)

    @property
    def visitable(self) -> bool:
        return self.__visitable

    def string_crtp(self, crtp_param: str = ""):
        if not self.__crtp and len(crtp_param) != 0:
            raise RuntimeError("Not a CRTP base, but CRTP parameter is provided")

        return "{} {}<{}>".format(self.__accessibility, self.__name, crtp_param)


class ParserClassTypeEnum:

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
    __enum: ParserClassTypeEnum

    def __init__(self, name: str, classes: list[ParserClass]):
        self.__name = name
        self.__classes = classes
        self.__enum = ParserClassTypeEnum(list(c.name for c in self.__classes))

    @property
    def primary_lines(self) -> str:
        global bases
        yield "namespace {} {{".format(self.__name)

        for b in bases:
            if b.visitable:
                visitor = b.get_visitor()
                for l1 in visitor.primary_lines:
                    yield l1

        for c in self.__classes:
            for line in c.lines:
                yield line

        # for ac in self.__visitor.accept_func:
        #     yield ac
        for b in bases:
            if b.visitable:
                visitor = b.get_visitor()
                for l1 in visitor.accept_func:
                    yield l1

        yield "}"

    @property
    def secondary_lines(self):
        yield "namespace {} {{".format(self.__name)

        for e in self.__enum.lines:
            yield e

        # for l1 in self.__visitor.secondary_lines:
        #     yield l1
        for b in bases:
            if b.visitable:
                visitor = b.get_visitor()
                for l1 in visitor.secondary_lines:
                    yield l1

        yield "}"


bases: list[BaseClass] = list()


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
    global bases

    with open(str(args.config[0]), "r") as conf:
        conf_dict: dict = json.load(conf)

        expr_classes: dict = conf_dict[JSON_EXPR_CLASSES_NAME]

        res = list(ParserClass(expr_cls_def["name"], expr_cls_def["member"],
                               (b for b in bases if b.id in expr_cls_def["base"]))
                   for expr_cls_def in
                   expr_classes)

        for r in res:
            r.add_to_parent()

        return res


def root_namespace(args: argparse):
    global bases

    with open(str(args.config[0]), "r") as conf:
        conf_dict: dict = json.load(conf)

        base_classes_dict: dict = conf_dict[JSON_CLASS_BASE_NAME]
        bases = list(BaseClass(b["name"], b["access"], b["crtp"], b["visitable"], b["id"]) for b in base_classes_dict)

        namespace_conf: str = conf_dict[JSON_NAMESPACE_NAME]

        classes = expression_classes(args)
        return Namespace(namespace_conf, classes)


def write_back(path: str, head: list, content: list, tail: list):
    logging.info("Writing result to {}.".format(path))

    with open(path, "w") as f:
        for h in head:
            f.write("{}\n".format(h))

        for c in content:
            f.write("{}\n".format(c))

        for t in tail:
            f.write("{}\n".format(t))


def file_date_time(path: str) -> datetime.datetime:
    fname = pathlib.Path(path)
    return datetime.datetime.fromtimestamp(fname.stat().st_mtime)


def check_exist(path: str) -> bool:
    fname = pathlib.Path(path)
    return fname.exists()


def generate(args: argparse):
    logging.info("Head template file is {}.".format(args.head[0]))
    logging.info("Tail template file is {}.".format(args.tail[0]))
    logging.info("Configuration file is {}.".format(args.config[0]))
    logging.info("Target primary file is {}.".format(args.primary[0]))
    logging.info("Target secondary file is {}.".format(args.secondary[0]))

    if all({check_exist(args.primary[0]), check_exist(args.secondary[0])}):
        config_file_time: list[datetime.datetime] = list(
            {file_date_time(args.head[0]), file_date_time(args.tail[0]), file_date_time(args.config[0])})

        primary_time = file_date_time(args.primary[0])
        secondary_time = file_date_time(args.secondary[0])

        if all(primary_time > t for t in config_file_time) and all(secondary_time > t for t in config_file_time):
            logging.info("Everything updated.")
            exit(0)

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
    # Logging to screen
    formatter = logging.Formatter('%(message)s')
    logging.getLogger('').setLevel(logging.DEBUG)
    main()
