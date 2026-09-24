import platform
from argparse import Namespace

from settings.config import DEPENDENCIES, UPDATE_PACKAGE_REPOS_COMMAND, INSTALL_COMMAND, LIBPQXX_PACKAGE
from settings.command.detail.command import Command
from settings.command.detail.errors import NotSupportedOperatingSystem
from settings.command.detail.options import MANUAL_OPTION
from settings.command.detail.postgres import Postgres


class Install(Command):
    def __init__(self):
        Command.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "install"

    def command_explicit(self, args: Namespace) -> list[list[str]]:
        match platform.system():
            case "Linux":
                return [UPDATE_PACKAGE_REPOS_COMMAND.split(" "), f"{INSTALL_COMMAND} {LIBPQXX_PACKAGE}".split(" ")]
            case _:
                raise NotSupportedOperatingSystem()

    def details(self) -> str:
        return "Installs dependencies needed for the project.\n\n" \
               "You do not need to install dependencies if you already have them. " \
               "Here is the list of every dependencies needed:\n" \
               f"{"".join(f"\t-{dep}\n" for dep in DEPENDENCIES)}\n"

    def setup(self, args: Namespace) -> None:
        Postgres().setup()

    def teardown(self, args: Namespace) -> None:
        pass