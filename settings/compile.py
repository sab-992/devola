import os
import platform
from argparse import Namespace
from settings.helper.command import Command
from settings.helper.directory import DirectoryChanger
from settings.helper.options import DEBUG_OPTION, MANUAL_OPTION
from settings.helper.config import POSTGRE_INSTALLATION_PATH
from settings.debug import Debug

class CMake(Command, DirectoryChanger):
    def __init__(self):
        pass

    def arguments(self) -> dict[str, dict]:
        return { "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "cmake"

    def command_explicit(self, args: Namespace) -> str:
        return f"{self.change_to_build_dir_command()} && cmake {f"-DPostgreSQL_ROOT=\"{POSTGRE_INSTALLATION_PATH}\" " if platform.system() == "Windows" else ""}-DCMAKE_BUILD_TYPE=Debug .. && {self.reset_directory_command()}"
    
    def details(self) -> str:
        return "Use the CMakeLists.txt to prepare the environment for the application.\n\n" \
               "For windows, make sure you have postgres installed, and that the path in " \
               "\"settings/helper/config.py\" for the POSTGRE_INSTALLATION_PATH matches your current installation path."

class Make(Command, DirectoryChanger):
    def __init__(self):
        self.__debug = Debug()

    def arguments(self) -> dict[str, dict]:
        return { "debug": DEBUG_OPTION, "manual": MANUAL_OPTION } 

    def command(self) -> str:
        return "make"

    def command_explicit(self, args: Namespace) -> str:
        return f"{self.change_to_build_dir_command()} && cmake --build . && {self.reset_directory_command()}{f" && {self.__debug.command_explicit(args)}" if args.debug else ""}"
    
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
        return f"{self.__cmake.command_explicit(args)} && {self.__make.command_explicit(args)}"
    
    def details(self) -> str:
        return "Combines the 'cmake' command and the 'make' command to prepare the application environment and build it."