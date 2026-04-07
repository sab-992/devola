import platform
import subprocess
from argparse import Namespace

from settings.command.detail.command import Command
from settings.command.detail.options import CLEAN_OPTION, MANUAL_OPTION


class Shutdown(Command):
    def __init__(self):
        Command.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "clean": CLEAN_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "shutdown"

    def command_explicit(self, args: Namespace) -> list[list[str]]:
        return [["docker", "system", "prune", "-a", "--volumes"]] if args.clean else []

    def details(self) -> str:
        return "Stops and removes ALL locally running containers."
    
    def setup(self, args: Namespace) -> str:
        ids = subprocess.check_output(["docker", "ps", "-q"]).decode().split()

        if ids:
            subprocess.run(["docker", "stop"] + ids)

    def teardown(self, args: Namespace) -> str:
        pass