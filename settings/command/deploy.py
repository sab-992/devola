import os
import subprocess
from argparse import Namespace

from settings import config
from settings.config import SERVICES_PATH
from settings.command.detail.command import Command
from settings.command.detail.file_system import FileSystem
from settings.command.detail.log import log, Color
from settings.command.detail.service_updater import ServiceUpdater
from settings.command.detail.options import PROD_OPTION, OUTPUT_OPTION, MANUAL_OPTION


class Deploy(Command, ServiceUpdater):
    def __init__(self):
        Command.__init__(self)
        ServiceUpdater.__init__(self)
        self.__fs = FileSystem()
        self.env = os.environ.copy()

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
        log("Starting development environment...", False, Color.PURPLE)

        root_folder_path = self.__fs.find_root_folder()
        docker_folder = f"{root_folder_path}/docker"
        self.create_base_docker_file(args, docker_folder)

        services: list[str] = self.get_services()

        self.start_base_containers(args, docker_folder)
        self.starting_database_containers(services, root_folder_path)
        self.start_service_containers(services, root_folder_path)

        log(f"Deployment complete.", False, Color.GREEN)
        log(f"Currently running containers:", False, Color.NC)

    def create_base_docker_file(self, args: Namespace, docker_folder: str):
        log("Generating docker compose...", False, Color.PURPLE)

        cmd = ["python", f"{docker_folder}/generate_docker_file.py"]
        if args.prod:
            cmd += ["-p"]

        subprocess.run(cmd, env=self.env);

    def starting_database_containers(self, services: list[str], root_folder_path: str):
        for i in range(len(services)):
            service_docker_dir = f"{root_folder_path}/{SERVICES_PATH}/{services[i]}/docker"
            files = self.__fs.get_files(service_docker_dir)

            for file in files:
                if not "database" in file:
                    continue

                log(f"Deploying database for {file}...", False, Color.PURPLE)
                subprocess.run(["docker", "compose", "-f", f"{service_docker_dir}/{file}", "up", "-d", "--build"], env=self.env)

    def start_service_containers(self, services: list[str], root_folder_path: str):
        for i in range(len(services)):
            log(f"Deploying {services[i]}...", False, Color.PURPLE)

            env_copy = self.env.copy()
            env_copy["SERVICE_NAME"]=f"{services[i]}"

            subprocess.run(["docker", "compose", "-p", f"{services[i]}", "-f",
                            f"{root_folder_path}/{SERVICES_PATH}/{services[i]}/docker/docker-compose.{services[i]}.yml", "up", "-d", "--build"], env=env_copy)

    def start_base_containers(self, args: Namespace, docker_folder: str):
        log(f"Starting base containers...", False, Color.PURPLE)

        cmd = ["docker", "compose", "-f", f"{docker_folder}/docker-compose.yml", "up", "-d", "--build",
               f"{config.FRONT_END_DOCKER_SERVICE_NAME}", f"{config.NGINX_DOCKER_SERVICE_NAME}"]
        if not args.prod:
            cmd += [f"{config.PGADMIN_DOCKER_SERVICE_NAME}"]

        subprocess.run(cmd, env=self.env)