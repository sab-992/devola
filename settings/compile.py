from shlex import quote
from argparse import Namespace
from settings.helper.command import Command
from settings.helper.directory import DirectoryChanger
from settings.helper.options import DEBUG_OPTION, MANUAL_OPTION

class CMake(Command, DirectoryChanger):
    def __init__(self):
        pass

    def arguments(self) -> dict[str, dict]:
        return { "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "cmake"

    def command_explicit(self, args: Namespace) -> str:
        if args.manual:
            return f"echo {quote(self.manual())}"
        super().validate_arguments(args)
        return f"{self.change_to_build_dir_command()} cmake -DCMAKE_BUILD_TYPE=Debug ..; {self.reset_directory_command()}"
    
    def details(self) -> str:
        return "Use the CMakeLists.txt to prepare the environment for the application."

class Make(Command, DirectoryChanger):
    def __init__(self):
        pass

    def arguments(self) -> dict[str, dict]:
        return { "debug": DEBUG_OPTION, "manual": MANUAL_OPTION } 

    def command(self) -> str:
        return "make"

    def command_explicit(self, args: Namespace) -> str:
        if args.manual:
            return f"echo {quote(self.manual())}"
        super().validate_arguments(args)
        return f"{self.change_to_build_dir_command()} cmake --build . --parallel $(nproc); {"./devola; " if args.debug else ""} {self.reset_directory_command()}"
    
    def details(self) -> str:
        return "Use the environment made by the 'cmake' command and build/compiles the application."

class MakeAll(Command, DirectoryChanger):
    def __init__(self):
        self.__make = Make()
        self.__cmake = CMake()

    def arguments(self) -> dict[str, dict]:
        return { "debug": DEBUG_OPTION, "manual": MANUAL_OPTION } 

    def command(self) -> str:
        return "makeall"

    def command_explicit(self, args: Namespace) -> str:
        if args.manual:
            return f"echo {quote(self.manual())}"
        return f"{self.__cmake.command_explicit(args)} {self.__make.command_explicit(args)}"
    
    def details(self) -> str:
        return "Combines the 'cmake' command and the 'make' command to prepare the application environment and build it."