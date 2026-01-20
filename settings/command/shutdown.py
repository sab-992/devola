from argparse import Namespace

from settings.command.detail.command import Command
from settings.command.detail.options import CLEAN_OPTION, MANUAL_OPTION


REMOVE_DOCKER_CONTAINERS_CMD = "docker rm -f $(docker ps -aq)"
REMOVE_DOCKER_IMAGES_CMD = "docker rmi -f $(docker images -q)"
REMOVE_DOCKER_NETWORK_CMD = "docker network rm app-network"
CLEAN_ALL_CMD = "docker system prune -a --volumes"

class Shutdown(Command):
    def __init__(self):
        Command.__init__(self)

    def arguments(self) -> dict[str, dict]:
        return { "clean": CLEAN_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "shutdown"

    def command_explicit(self, args: Namespace) -> str:
        return f"{REMOVE_DOCKER_CONTAINERS_CMD};\
                 {REMOVE_DOCKER_IMAGES_CMD};\
                 {REMOVE_DOCKER_NETWORK_CMD}{f"; {CLEAN_ALL_CMD}" if args.clean else ""}"

    def details(self) -> str:
        return "Stops and removes ALL locally running containers."
    
    def setup(self, args: Namespace) -> str:
        pass

    def teardown(self, args: Namespace) -> str:
        pass