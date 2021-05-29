import argparse
import pathlib


def generate_classes(arg_parser: argparse):
    pass


def main():
    def validate_config_file(arg: str) -> str:
        path: pathlib.Path = pathlib.Path(arg)
        if path.exists():
            return str(path.absolute().as_posix())
        else:
            raise argparse.ArgumentError("FATAL: path {} not exists".format(arg))

    def validate_target_file(arg: str) -> str:
        pass

    arg_parser: argparse = argparse.ArgumentParser(description="Generator for parser classes")

    arg_parser.add_argument('-c', '--config', type=lambda x: validate_config_file(x), nargs=1,
                            help='configuration file')

    arg_parser.add_argument('-g', '--generate', type=lambda x: validate_target_file(x), nargs=1,
                            help='configuration file')

    generate_classes(arg_parser)


if __name__ == "__main__":
    main()
