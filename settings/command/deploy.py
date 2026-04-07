import os
import subprocess
from argparse import Namespace

from settings.command.detail import config
from settings.command.detail.command import Command
from settings.command.detail.config import SERVICES_PATH
from settings.command.detail.file_system import FileSystem
from settings.command.detail.log import log, Color
from settings.command.detail.service_updater import ServiceUpdater
from settings.command.detail.options import PROD_OPTION, OUTPUT_OPTION, MANUAL_OPTION


# TODO: Fix deploy and docker files
class Deploy(Command, ServiceUpdater):
    def __init__(self):
        Command.__init__(self)
        ServiceUpdater.__init__(self)
        self.__fs = FileSystem()

    def arguments(self) -> dict[str, dict]:
        return { "output": OUTPUT_OPTION, "prod": PROD_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "deploy"

    def command_explicit(self, args: Namespace) -> list[list[str]]:
        commands = [["docker", "ps"]]

        if args.output:
            commands += ["docker", "logs", "-f", f"{str(args.output)}"]

        return commands
    
    def details(self) -> str:
        return "Deploy the application in docker containers."
    
    def setup(self, args: Namespace) -> str:
        self.update_services()
        self.__deploy(args)

    def teardown(self, args: Namespace) -> str:
        pass
    
    def __deploy(self, args: Namespace):
        services_name = self.get_services()
        env = os.environ.copy()
        root_folder_path = self.__fs.find_root_folder()
        docker_folder = f"{root_folder_path}/docker"

        # Creating Docker Compose file :
        log("Generating docker compose...", False, Color.PURPLE)
        generate_docker_compose_cmd = f"python {docker_folder}/generate_docker_file.py{" -p" if args.prod else ""}"
        subprocess.run(generate_docker_compose_cmd, shell=True, env=env)
        
        # Starting Database container :
        log("Starting development environment...", False, Color.PURPLE)
        starting_db_cmd = f"docker compose -f {docker_folder}/docker-compose.yml up -d --build {config.DATABASE_DOCKER_SERVICE_NAME}"
        subprocess.run(starting_db_cmd, shell=True, env=env)
        
        # Starting each service's container :
        for i in range(len(services_name)):
            deploy_services_cmd = f"SERVICE_NAME={services_name[i]} docker compose -p {services_name[i]} -f {root_folder_path}/{SERVICES_PATH}/{services_name[i]}/docker/docker-compose.{services_name[i]}.yml up -d --build"
            log(f"Deploying {services_name[i]}...", False, Color.PURPLE)
            subprocess.run(deploy_services_cmd, shell=True, env=env)

        # Starting Front-end, nginx (and pgadmin) containers :
        log(f"Starting other containers...", False, Color.PURPLE)
        starting_other_containers = f"docker compose -f {docker_folder}/docker-compose.yml up -d --build {config.FRONT_END_DOCKER_SERVICE_NAME} {config.NGINX_DOCKER_SERVICE_NAME}{"" if args.prod else f" {config.PGADMIN_DOCKER_SERVICE_NAME}"};"
        subprocess.run(starting_other_containers, shell=True, env=env)

        log(f"Deployment complete.", False, Color.GREEN)
        log(f"Currently running containers:", False, Color.NC)