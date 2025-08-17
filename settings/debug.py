from argparse import Namespace
from settings.helper.command import Command
from settings.helper.config import DEBUG_EXECUTABLE_PATH_FROM_BUILD_DIR
from settings.helper.options import MANUAL_OPTION

class Debug(Command):
    def __init__(self):
        pass

    def arguments(self) -> dict[str, dict]:
        return { "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "debug"

    def command_explicit(self, args: Namespace) -> str:
        if args.manual:
            return f"echo \"{self.manual()}\""
        super().validate_arguments(args)
        return f"{DEBUG_EXECUTABLE_PATH_FROM_BUILD_DIR}"
    
    def details(self) -> str:
        return "Starts the application."