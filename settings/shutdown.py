from argparse import Namespace
from settings.helper.command import Command
from settings.helper.options import CLEAN_OPTION, MANUAL_OPTION

REMOVE_DOCKER_CONTAINERS_CMD = "docker rm -f $(docker ps -aq)"
REMOVE_DOCKER_IMAGES_CMD = "docker rmi -f $(docker images -q)"
REMOVE_DOCKER_NETWORK_CMD = "docker network rm app-network"
CLEAN_ALL_CMD = "docker system prune -a --volumes"

class Shutdown(Command):
    def __init__(self):
        pass

    def arguments(self) -> dict[str, dict]:
        return { "clean": CLEAN_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "shutdown"

    def command_explicit(self, args: Namespace) -> str:
        if args.manual:
            return f"echo {self.manual()}"
        super().validate_arguments(args)
        return f"{REMOVE_DOCKER_CONTAINERS_CMD};\
                 {REMOVE_DOCKER_IMAGES_CMD};\
                 {REMOVE_DOCKER_NETWORK_CMD}{f"; {CLEAN_ALL_CMD}" if args.clean else ""}"

    def manual(self) -> str:
        return ""