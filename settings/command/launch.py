import platform
from argparse import Namespace

from settings.command.detail.command import Command
from settings.command.detail.directory import Directory
from settings.command.detail.options import MANUAL_OPTION


class Launch(Command, Directory):
    def __init__(self):
        Command.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "launch"

    def command_explicit(self, args: Namespace) -> list[list[str]]:
        return [[self.uniformizePath("server/Debug/dev_server.exe" if platform.system() == "Windows" else "./server/dev_server")]]
    
    def details(self) -> str:
        return "Starts the application."
    
    def setup(self, args: Namespace) -> str:
        self.set_working_directory(self.build_directory())

    def teardown(self, args: Namespace) -> str:
        self.reset_working_directory()