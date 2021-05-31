import argparse
import pathlib
import json

JSON_EXPR_CLASSES_NAME: str = "expression_classes"


class ExpressionClassMemberDef:
    __type: str
    __name: str

    def __init__(self, _name: str, _type: str):
        __name = _name
        __type = _type

    def get_definition(self) -> str:
        return "{0} {1}{{}};".format(self.__type, self.__name)
        pass

    def get_getter(self):
        yield "[[nodiscard]] get_{}(){".format(self.__name[:-1])
        yield "return this->{};}".format(self.__name)
        pass

    def get_setter(self):
        yield "void set_{0}(const {1}&{0}){".format(self.__name[:-1], self.__type)
        yield "{}={};}".format(self.__name, self.__name[:-1])


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

        self.__def += "{{}}"

    def get_definition(self) -> str:
        return self.__def


class ExpressionClass:
    __name: str
    __members: list[ExpressionClassMemberDef]

    __ctor: ExpressionClassConstructorDef

    def __init__(self, _name: str, members_: dict):
        __name = _name
        __members = list(ExpressionClassMemberDef(m["name"], m["type"]) for m in members_)

    def lines(self):
        yield "class {0}{{".format(self.__name)

        yield "public:"

        yield "{0}(const {0}&)=default;".format(self.__name)
        yield "{0}({0}&&)=default;".format(self.__name)
        yield "{0}& operator=(const {0}&)=default;".format(self.__name)

        yield self.__ctor.get_definition()

        for m in self.__members:
            for s in m.get_getter():
                yield s

        for m in self.__members:
            for s in m.get_setter():
                yield s

        yield "private:"

        for m in self.__members:
            yield m.get_definition()

        yield "}};"


def expression_classes(arg_parser: argparse):
    with open(str(arg_parser.config[0]), "r") as conf:
        conf_dict: dict = json.load(conf)

        expr_classes: dict = conf_dict[JSON_EXPR_CLASSES_NAME]

        for cls in ((ExpressionClass(expr_cls_def["name"], expr_cls_def["member"])) for expr_cls_def in expr_classes):
            yield cls


def write_back(arg_parser: argparse, head: list, content: list, tail: list):
    with open(arg_parser.target[0], "w") as f:
        for h in head:
            f.write("{}\n".format(h))

        for c in content:
            f.write("{}\n".format(c))

        for t in tail:
            f.write("{}\n".format(t))


def verify(arg_parser: argparse):
    pass


def generate(arg_parser: argparse):
    head: list = list()
    with open(arg_parser.template[0], "r") as template:
        line = template.readline()
        while line.strip() != "!":
            head.append(line)
            line = template.readline()
            if not line:
                raise ValueError("template file is invalid")

        tail: list = list()
        line = template.readline()
        while line.strip() != "!":
            tail.append(line)
            line = template.readline()

    content: list[str] = list()

    for cls in expression_classes(arg_parser):
        for line in cls.lines():
            content.append(line)

    write_back(arg_parser, head, content, tail)

    verify(arg_parser)


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
        if not path.is_file():
            raise argparse.ArgumentError("FATAL:{} should be a file".format(arg))

        return str(path.absolute().as_posix())

    arg_parser: argparse = argparse.ArgumentParser(description="Generator for parser classes")

    arg_parser.add_argument('-c', '--config', type=lambda x: validate_config_file(x), nargs=1,
                            help='configuration file', required=True)

    arg_parser.add_argument('-t', '--template', type=lambda x: validate_config_file(x), nargs=1,
                            help='configuration file', required=True)

    arg_parser.add_argument('-t', '--target', type=lambda x: validate_target_file(x), nargs=1,
                            help='target configuration file', required=True)

    generate(arg_parser)


if __name__ == "__main__":
    main()
