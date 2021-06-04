import argparse
import pathlib
import json
import logging
import datetime

JSON_EXPR_CLASSES_NAME: str = "expression_classes"
JSON_INC_NAME: str = "includes"
JSON_NAMESPACE_NAME: str = "generated_namespace"
JSON_CLASS_BASE_NAME: str = "expression_class_bases"


class BaseClass:
    __name: str
    __accessibility: str

    def __init__(self, name: str, acc: str):
        self.__name = name
        self.__accessibility = acc

    @property
    def string(self):
        return "{} {}".format(self.__accessibility, self.__name)


def concat_base_classes(classes: list[BaseClass]) -> str:
    ret: str = ''
    for c in classes:
        ret += (c.string + ",")
    return ret[:-1]


class IncludeHeader:
    __name: str

    def __init__(self, name: str):
        self.__name = name

    @property
    def line(self) -> str:
        return "#include <{}>".format(self.__name)


class ExpressionClassMemberDef:
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


class ExpressionClassConstructorDef:
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


class ExpressionClass:
    __name: str = ""
    __members: list[ExpressionClassMemberDef] = list()
    __parents: list[BaseClass] = list()

    __ctor: ExpressionClassConstructorDef

    def __init__(self, _name: str, members_: dict, base: dict):
        self.__name = _name
        self.__members = list(ExpressionClassMemberDef(m["name"], m["type"]) for m in members_)

        self.__parents = list(BaseClass(b["name"], b["access"]) for b in base)

        self.__ctor = ExpressionClassConstructorDef(_name, members_)

    @property
    def name(self) -> str:
        return self.__name

    @property
    def lines(self):
        yield "class {0}:{1}{{".format(self.__name, concat_base_classes(self.__parents))

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

        yield "private:"

        for m in self.__members:
            yield m.definition

        yield "};"


class Namespace:
    __name: str

    def __init__(self, name: str, classes):
        self.__name = name
        self.__classes = classes

    @property
    def lines(self) -> str:
        yield "namespace {} {{".format(self.__name)

        for c in self.__classes:
            for line in c.lines:
                yield line

        yield "}"


def include_files(args: argparse):
    with open(str(args.config[0]), "r") as conf:
        conf_dict: dict = json.load(conf)

        includings: dict = conf_dict[JSON_INC_NAME]

        return (IncludeHeader(h) for h in includings)


def expression_classes(args: argparse):
    with open(str(args.config[0]), "r") as conf:
        conf_dict: dict = json.load(conf)

        expr_classes: dict = conf_dict[JSON_EXPR_CLASSES_NAME]
        base_classes: dict = conf_dict[JSON_CLASS_BASE_NAME]

        return (ExpressionClass(expr_cls_def["name"], expr_cls_def["member"], base_classes) for expr_cls_def in
                expr_classes)


def root_namespace(args: argparse):
    with open(str(args.config[0]), "r") as conf:
        conf_dict: dict = json.load(conf)

        namespace_conf: str = conf_dict[JSON_NAMESPACE_NAME]

        expr_classes: dict = conf_dict[JSON_EXPR_CLASSES_NAME]

        return Namespace(namespace_conf, expression_classes(args))


def write_back(args: argparse, head: list, content: list, tail: list):
    logging.info("Writing result to {}.".format(args.output[0]))

    with open(args.output[0], "w") as f:
        for h in head:
            f.write("{}\n".format(h))

        for c in content:
            f.write("{}\n".format(c))

        for t in tail:
            f.write("{}\n".format(t))


def verify(arg_parser: argparse):
    logging.info("Verifying result {}.".format(arg_parser.output[0]))
    # TODO
    logging.info("Succeeded on ".format(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")))


def generate(args: argparse):
    logging.info("Head template file is {}.".format(args.head[0]))
    logging.info("Tail template file is {}.".format(args.tail[0]))

    head: list = list()
    head.append("#pragma once")

    for h in include_files(args):
        head.append(h.line)

    with open(args.head[0], "r") as template:
        line = template.readline()
        while len(line) != 0:
            head.append(line.strip())
            line = template.readline()

    tail: list = list()
    with open(args.tail[0], "r") as template:
        line = template.readline()
        while len(line) != 0:
            tail.append(line.strip())
            line = template.readline()

    content: list[str] = list(root_namespace(args).lines)

    write_back(args, head, content, tail)

    verify(args)


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

    arg_parser.add_argument('-o', '--output', type=lambda x: validate_target_file(x), nargs=1,
                            help='target configuration file', required=True)

    generate(arg_parser.parse_args())


if __name__ == "__main__":
    main()
