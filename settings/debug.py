from argparse import Namespace
import platform
from settings.helper.directory import DirectoryChanger
from settings.helper.command import Command
from settings.helper.config import DEBUG_EXECUTABLE_PATH_FROM_BUILD_DIR
from settings.helper.options import MANUAL_OPTION

class Debug(Command, DirectoryChanger):
    def __init__(self):
        pass

    def arguments(self) -> dict[str, dict]:
        return { "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "debug"

    def command_explicit(self, args: Namespace) -> str:
        build_path = DEBUG_EXECUTABLE_PATH_FROM_BUILD_DIR
        return f"{self.change_to_build_dir_command()} && {f"{build_path.replace('/', '\\')}.exe"if platform.system() == "Windows" else build_path} && {self.reset_directory_command()}"
    
    def details(self) -> str:
        return "Starts the application."