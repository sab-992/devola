from argparse import Namespace
from settings.helper.command import Command
from settings.helper.directory import DirectoryChanger
from settings.helper.options import REGEX_OPTION, MANUAL_OPTION

class Test(Command, DirectoryChanger):
    def __init__(self):
        pass

    def arguments(self) -> dict[str, dict]:
        return { "regex": REGEX_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "test"

    def command_explicit(self, args: Namespace) -> str:
        return f"{self.change_to_build_dir_command()} cmake -DCMAKE_BUILD_TYPE=Tests ..; cmake --build . --parallel $(nproc) {f"--target run_all_tests" if not args.regex else ""}; {f"./tests/tests --gtest_filter=\"{args.regex}\";" if args.regex else ""} {self.reset_directory_command()}"
    
    def details(self) -> str:
        return "Launches automated tests."