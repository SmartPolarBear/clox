import argparse
import pathlib


def parse_config(arg_parser: argparse) -> list:
    pass


def write_back(arg_parser: argparse, head: list, content: list, tail: list):
    pass


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

    content: list = parse_config(arg_parser)

    write_back(arg_parser, head, content, tail)

    verify(arg_parser)

    pass


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

    arg_parser.add_argument('-g', '--generate', type=lambda x: validate_target_file(x), nargs=1,
                            help='configuration file', required=True)

    generate(arg_parser)


if __name__ == "__main__":
    main()
