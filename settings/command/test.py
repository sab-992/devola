import os
import platform
from argparse import Namespace

from settings.command.detail.command import Command
from settings.command.detail.directory import Directory
from settings.command.detail.options import REGEX_OPTION, MANUAL_OPTION
from settings.command.detail.service_updater import ServiceUpdater


# Rework this to match new file structure and add the new "launch private tests" feature
class Test(Command, Directory, ServiceUpdater):
    def __init__(self):
        Command.__init__(self)
        ServiceUpdater.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "regex": REGEX_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "test"

    def command_explicit(self, args: Namespace) -> list[list[str]]:
        if platform.system() == "Windows":
            path = "test/Debug/tests.exe"
            # Fallback case (might be GNU compiler on Windows)
            if not os.path.isfile(path):
                path = "test/tests.exe"
        else:
            path = "./test/tests"

        command: list[str] = [self.uniformizePath(path)]

        if args.regex:
            command.append(f"--gtest_filter={args.regex}")

        return [command]

    def details(self) -> str:
        return "Launches automated tests."

    def setup(self, args: Namespace) -> None:
        self.set_working_directory(self.build_directory())

    def teardown(self, args: Namespace) -> None:
        self.reset_working_directory()