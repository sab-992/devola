import platform
from argparse import Namespace

from settings.command.debug import Debug
from settings.command.detail.command import Command
from settings.command.detail.config import POSTGRE_INSTALLATION_PATH
from settings.command.detail.directory import Directory
from settings.command.detail.options import DEBUG_OPTION, MANUAL_OPTION
from settings.command.detail.service_updater import ServiceUpdater


class CMake(Command, Directory, ServiceUpdater):
    def __init__(self):
        Command.__init__(self)
        ServiceUpdater.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "cmake"

    def command_explicit(self, args: Namespace) -> str:
        return f"cmake {f"-DPostgreSQL_ROOT=\"{POSTGRE_INSTALLATION_PATH}\" " if platform.system() == "Windows" else ""}-DCMAKE_BUILD_TYPE=debug .."
    
    def details(self) -> str:
        return "Use the CMakeLists.txt to prepare the environment for the application.\n\n" \
               "For windows, make sure you have postgres installed, and that the path in " \
               "\"settings/helper/config.py\" for the POSTGRE_INSTALLATION_PATH matches your current installation path."
    
    def setup(self, args: Namespace) -> str:
        self.update_services()
        self.set_working_directory(self.build_directory())

    def teardown(self, args: Namespace) -> str:
        pass


class Make(Command, Directory, ServiceUpdater):
    def __init__(self):
        Command.__init__(self)
        ServiceUpdater.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "debug": DEBUG_OPTION, "manual": MANUAL_OPTION } 

    def command(self) -> str:
        return "make"

    def command_explicit(self, args: Namespace) -> str:
        return f"cmake --build .{f" && {Debug().command_explicit(args)}" if args.debug else ""}"
    
    def details(self) -> str:
        return "Use the environment made by the 'cmake' command and build/compiles the application."
    
    def setup(self, args: Namespace) -> str:
        self.update_services()
        self.set_working_directory(self.build_directory())

    def teardown(self, args: Namespace) -> str:
        pass

class MakeAll(Command, Directory, ServiceUpdater):
    def __init__(self):
        Command.__init__(self)
        ServiceUpdater.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "debug": DEBUG_OPTION, "manual": MANUAL_OPTION } 

    def command(self) -> str:
        return "makeall"

    def command_explicit(self, args: Namespace) -> str:
        return f"{CMake().command_explicit(args)} && {Make().command_explicit(args)}"
    
    def details(self) -> str:
        return "Combines the 'cmake' command and the 'make' command to prepare the application environment and build it."

    def setup(self, args: Namespace) -> str:
        self.update_services()
        self.set_working_directory(self.build_directory())

    def teardown(self, args: Namespace) -> str:
        pass