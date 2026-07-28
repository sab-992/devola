import os
import platform
from argparse import Namespace

from settings.command.detail.command import Command
from settings.command.detail.postgres import Postgres
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
        Postgres().run()

        if platform.system() == "Windows":
            path = "server/Debug/dev_server.exe"
            # Fallback case (might be GNU compiler on Windows)
            if not os.path.isfile(path):
                path = "server/dev_server.exe"
        else:
            path = "./server/dev_server"

        return [[self.uniformizePath(path)]]

    def details(self) -> str:
        return "Starts the application."

    def setup(self, args: Namespace) -> str:
        self.set_working_directory(self.build_directory())

    def teardown(self, args: Namespace) -> str:
        self.reset_working_directory()