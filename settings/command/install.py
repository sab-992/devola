import platform
from argparse import Namespace

from settings.config import DEPENDENCIES, USE_VCPKG, UPDATE_PACKAGE_REPOS_COMMAND, INSTALL_COMMAND, LIBPQXX_PACKAGE
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
        cmd = [];
        match platform.system():
            case "Windows":
                if (USE_VCPKG):
                    cmd = [["vcpkg",  "install", "libpq"]]
                else:
                    cmd = [["choco", "install", "postgresql", "--yes"]]
            case "Linux":
                cmd = [UPDATE_PACKAGE_REPOS_COMMAND.split(" "), f"{INSTALL_COMMAND} {LIBPQXX_PACKAGE}".split(" ")]
            case _:
                raise NotSupportedOperatingSystem()
        return cmd

    def details(self) -> str:
        return "Installs dependencies needed for the project.\n\n" \
               "You do not need to install dependencies if you already have them. " \
               "Here is the list of every dependencies needed:\n" \
               f"{"".join(f"\t-{dep}\n" for dep in DEPENDENCIES)}\n" \
               "For Windows, you can chose either to install using 'chocolatey' (default) or 'vcpkg'.\n" \
               "In order to do this, go in the config.py and change 'USE_VCPKG' to 'True' to use either one of them."

    def setup(self, args: Namespace) -> str:
        Postgres().setup()

    def teardown(self, args: Namespace) -> str:
        pass