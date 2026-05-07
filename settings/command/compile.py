import platform
from argparse import Namespace

from settings.config import POSTGRE_INSTALLATION_PATH
from settings.command.launch import Launch
from settings.command.test import Test
from settings.command.detail.command import Command
from settings.command.detail.directory import Directory
from settings.command.detail.file_system import FileSystem
from settings.command.detail.options import LAUNCH_OPTION, MANUAL_OPTION
from settings.command.detail.service_updater import ServiceUpdater


class CMake(Command, Directory, ServiceUpdater):
    def __init__(self):
        Command.__init__(self)
        ServiceUpdater.__init__(self)
        self.__fs = FileSystem()

    def arguments(self) -> dict[str, dict]:
        return { "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "cmake"

    def command_explicit(self, args: Namespace, build_type: str="debug") -> list[list[str]]:
        cmake_command = ["cmake"]

        if platform.system() == "Windows":
            cmake_command.append(f"-DPostgreSQL_ROOT={POSTGRE_INSTALLATION_PATH}")

        return [cmake_command + [f"-DCMAKE_BUILD_TYPE={build_type}", ".."] + self.__fs.extra_build_options()]

    def details(self) -> str:
        return "Use the CMakeLists.txt to prepare the environment for the application.\n\n" \
               "For windows, make sure you have postgres installed, and that the path in " \
               "\"settings/config.py\" for the POSTGRE_INSTALLATION_PATH matches your current installation path."

    def setup(self, args: Namespace) -> str:
        self.update_services()
        self.set_working_directory(self.build_directory())

    def teardown(self, args: Namespace) -> str:
        self.reset_working_directory()

class Make(Command, Directory, ServiceUpdater):
    def __init__(self):
        Command.__init__(self)
        ServiceUpdater.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "launch": LAUNCH_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "make"

    def command_explicit(self, args: Namespace, build_type="debug") -> list[list[str]]:
        make_command = ["cmake", "--build", "."]

        commands = [make_command]
        if args.launch and build_type == "debug":
            commands += Launch().command_explicit(args)

        return commands

    def details(self) -> str:
        return "Use the environment made by the 'cmake' command and build/compiles the application."

    def setup(self, args: Namespace) -> str:
        self.update_services()
        self.set_working_directory(self.build_directory())

    def teardown(self, args: Namespace) -> str:
        self.reset_working_directory()

class MakeAll(Command, Directory, ServiceUpdater):
    def __init__(self):
        Command.__init__(self)
        ServiceUpdater.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "launch": LAUNCH_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "makeall"

    def command_explicit(self, args: Namespace) -> list[list[str]]:
        return CMake().command_explicit(args) + Make().command_explicit(args)

    def details(self) -> str:
        return "Combines the 'cmake' command and the 'make' command to prepare the application environment and build it."

    def setup(self, args: Namespace) -> str:
        self.update_services()
        self.set_working_directory(self.build_directory())

    def teardown(self, args: Namespace) -> str:
        self.reset_working_directory()

class MakeTest(Command, Directory, ServiceUpdater):
    def __init__(self):
        Command.__init__(self)
        ServiceUpdater.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "launch": LAUNCH_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "maketest"

    def command_explicit(self, args: Namespace) -> list[list[str]]:
        commands = CMake().command_explicit(args, "test") + Make().command_explicit(args, "test")

        if args.launch:
            commands += Test().command_explicit(args)

        return commands

    def details(self) -> str:
        return "Compiles the google tests."

    def setup(self, args: Namespace) -> str:
        self.update_services()
        self.set_working_directory(self.build_directory())

    def teardown(self, args: Namespace) -> str:
        self.reset_working_directory()