import os
import subprocess
from settings.helper import config
from settings.helper.log import log, Color
from argparse import Namespace
from settings.helper.command import Command
from dotenv import dotenv_values
from settings.helper.log import Color, build_msg
from settings.helper.options import LOGS_OPTION, PROD_OPTION, MANUAL_OPTION

class Deploy(Command):
    def __init__(self):
        pass

    def arguments(self) -> dict[str, dict]:
        return { "logs": LOGS_OPTION, "prod": PROD_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "deploy"

    def command_explicit(self, args: Namespace) -> str:
        return self.__deploy(args)
    
    def details(self) -> str:
        return "Deploy the application in docker containers."
    
    def __deploy(self, args: Namespace) -> str:
        services_name = self.__get_services()
        env = os.environ.copy()

        # Creating Docker Compose file :
        log("Generating docker compose...", False, Color.GREEN)
        generate_docker_compose_cmd = f"python ./docker/generate_docker_file.py{" -p" if args.prod else ""}"
        subprocess.run(generate_docker_compose_cmd, shell=True, env=env)
        
        # Starting Database container :
        log("Starting development environment...", False, Color.GREEN)
        starting_db_cmd = f"docker compose -f {config.DOCKER_COMPOSE_FOLDER}/docker-compose.yml up -d --build {config.DATABASE_DOCKER_SERVICE_NAME}"
        subprocess.run(starting_db_cmd, shell=True, env=env)
        
        # Starting each service's container :
        for i in range(len(services_name)):
            deploy_services_cmd = f"SERVICE_NAME={services_name[i]} docker compose -p {services_name[i]} -f {config.SERVER_DOCKER_COMPOSE_FOLDER}/docker-compose.service.yml up -d --build"
            log(f"Deploying {services_name[i]}...", False, Color.GREEN)
            subprocess.run(deploy_services_cmd, shell=True, env=env)

        # Starting Front-end, nginx (and pgadmin) containers :
        log(f"Starting other containers...", False, Color.GREEN)
        starting_other_containers = f"docker compose -f {config.DOCKER_COMPOSE_FOLDER}/docker-compose.yml up -d --build {config.FRONT_END_DOCKER_SERVICE_NAME} {config.NGINX_DOCKER_SERVICE_NAME}{"" if args.prod else f" {config.PGADMIN_DOCKER_SERVICE_NAME}"};"
        subprocess.run(starting_other_containers, shell=True, env=env)

        log(f"Deployment complete.", False, Color.GREEN)
        log(f"Currently running containers:", False, Color.NC)
        end_cmd = f"docker ps"
        
        return f"{end_cmd}{f" && docker logs -f {str(args.logs)};" if args.logs else ""}"

    def __get_services(self) -> list[str]:
        services_name: list[str] = []
        env_vars = dotenv_values(".env")
        for key in env_vars.keys():
            if key.endswith(config.SERVICE_NAME_SUFFIX):
                services_name.append(env_vars.get(key))
        return services_name