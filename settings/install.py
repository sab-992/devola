import platform
from argparse import Namespace
from settings.helper.errors import NotSupportedOperatingSystem
from settings.helper.command import Command
from settings.helper.config import DEPENDENCIES, USE_VCPKG
from settings.helper.options import MANUAL_OPTION

class Install(Command):
    def __init__(self):
        pass

    def arguments(self) -> dict[str, dict]:
        return { "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "install"

    def command_explicit(self, args: Namespace) -> str:
        cmd = "";
        match platform.system():
            case "Windows":
                if (USE_VCPKG):
                    cmd = "vcpkg install libpq"
                else:
                    cmd = "choco install postgresql --yes"
            case "Linux":
                cmd = "sudo apt update; sudo apt install libpq-dev zlib1g-dev"
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