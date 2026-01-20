import platform
from argparse import Namespace

from settings.command.detail.command import Command
from settings.command.detail.config import DEBUG_EXECUTABLE_PATH_FROM_BUILD_DIR
from settings.command.detail.directory import Directory
from settings.command.detail.options import MANUAL_OPTION


class Debug(Command, Directory):
    def __init__(self):
        Command.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "debug"

    def command_explicit(self, args: Namespace) -> str:
        build_path = f"./{DEBUG_EXECUTABLE_PATH_FROM_BUILD_DIR}"
        return f"{f"{build_path.replace('/', '\\')}.exe"if platform.system() == "Windows" else build_path}"
    
    def details(self) -> str:
        return "Starts the application."
    
    def setup(self, args: Namespace) -> str:
        self.set_working_directory(self.build_directory())

    def teardown(self, args: Namespace) -> str:
        pass