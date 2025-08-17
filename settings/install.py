import platform
from argparse import Namespace
from settings.helper.errors import NotSupportedOperatingSystem
from settings.helper.command import Command
from settings.helper.options import MANUAL_OPTION

class Install(Command):
    def __init__(self):
        pass

    def arguments(self) -> dict[str, dict]:
        return { "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "install"

    def command_explicit(self, args: Namespace) -> str:
        if args.manual:
            return f"echo \"{self.manual()}\""
        super().validate_arguments(args)

        cmd = "";
        match platform.system():
            case "Windows":
                pass # todo
            case "Linux":
                cmd = "sudo apt update; sudo apt install libpq-dev"
            case _:
                raise NotSupportedOperatingSystem()
        return cmd
    
    def details(self) -> str:
        return "Installs dependencies needed for the project."