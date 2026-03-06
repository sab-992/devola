import platform
from argparse import Namespace

from settings.command.detail.command import Command
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
        if platform.system() != "Windows":
            return f"./server/dev_server"

        return "server/Debug/dev_server.exe".replace('/', '\\')
    
    def details(self) -> str:
        return "Starts the application."
    
    def setup(self, args: Namespace) -> str:
        self.set_working_directory(self.build_directory())

    def teardown(self, args: Namespace) -> str:
        pass