import platform
import subprocess
from argparse import Namespace

from settings.command.detail.command import Command
from settings.command.detail.options import CLEAN_OPTION, MANUAL_OPTION, VOLUMES_OPTION


class Shutdown(Command):
    def __init__(self):
        Command.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "clean": CLEAN_OPTION, "volumes": VOLUMES_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "shutdown"

    def command_explicit(self, args: Namespace) -> list[list[str]]:
        commands = [["docker", "ps"]]

        if args.clean:
            clean_command = ["docker", "system", "prune", "-a"]

            if args.volumes:
                clean_command.append("--volumes")

            commands.insert(0, clean_command)

        return commands

    def details(self) -> str:
        return "Stops ALL locally running containers."
    
    def setup(self, args: Namespace) -> str:
        ids = subprocess.check_output(["docker", "ps", "-q"]).decode().split()

        if ids:
            subprocess.run(["docker", "stop"] + ids)

    def teardown(self, args: Namespace) -> str:
        pass