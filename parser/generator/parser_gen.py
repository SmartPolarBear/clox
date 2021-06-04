import argparse
import pathlib
import json
import logging
import datetime

JSON_EXPR_CLASSES_NAME: str = "expression_classes"
JSON_INC_NAME: str = "includes"


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

    __ctor: ExpressionClassConstructorDef

    def __init__(self, _name: str, members_: dict):
        self.__name = _name
        self.__members = list(ExpressionClassMemberDef(m["name"], m["type"]) for m in members_)

        self.__ctor = ExpressionClassConstructorDef(_name, members_)

    @property
    def name(self) -> str:
        return self.__name

    @property
    def lines(self):
        yield "class {0}{{".format(self.__name)

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


def include_files(args: argparse):
    with open(str(args.config[0]), "r") as conf:
        conf_dict: dict = json.load(conf)

        includings: dict = conf_dict[JSON_INC_NAME]

        return (IncludeHeader(h) for h in includings)


def expression_classes(args: argparse):
    with open(str(args.config[0]), "r") as conf:
        conf_dict: dict = json.load(conf)

        expr_classes: dict = conf_dict[JSON_EXPR_CLASSES_NAME]

        return (ExpressionClass(expr_cls_def["name"], expr_cls_def["member"]) for expr_cls_def in expr_classes)


def write_back(args: argparse, head: list, content: list, tail: list):
    logging.info("Writing result to {}.".format(args.target[0]))

    with open(args.target[0], "w") as f:
        for h in head:
            f.write("{}\n".format(h))

        for c in content:
            f.write("{}\n".format(c))

        for t in tail:
            f.write("{}\n".format(t))


def verify(arg_parser: argparse):
    logging.info("Verifying result {}.".format(arg_parser.target[0]))
    # TODO
    logging.info("Succeeded on ".format(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")))


def generate(args: argparse):
    head: list = list()
    head.append("#pragma once")

    logging.info("Template file is {}.".format(args.template[0]))

    for h in include_files(args):
        head.append(h.line)

    with open(args.template[0], "r") as template:
        line = template.readline()
        while line.strip() != "!":
            head.append(line)
            line = template.readline()
            if not line:
                raise ValueError("template file is invalid")

        tail: list = list()
        line = template.readline()
        while line.strip() != "!" and len(line) != 0:
            tail.append(line)
            line = template.readline()

    content: list[str] = list()

    for cls in expression_classes(args):
        logging.info("Generating class {}.".format(cls.name))

        for line in cls.lines:
            content.append(line)

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

    arg_parser.add_argument('-m', '--template', type=lambda x: validate_config_file(x), nargs=1,
                            help='configuration file', required=True)

    arg_parser.add_argument('-t', '--target', type=lambda x: validate_target_file(x), nargs=1,
                            help='target configuration file', required=True)

    generate(arg_parser.parse_args())


if __name__ == "__main__":
    main()
