from shlex import quote
from settings.helper import config
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
        if args.manual:
            return f"echo {quote(self.manual())}"
        super().validate_arguments(args)
        return self.__deploy(args)
    
    def details(self) -> str:
        return "Deploy the application in docker containers."
    
    def __deploy(self, args: Namespace) -> str:
        services_name = self.__get_services()
        generate_docker_compose_cmd = f"echo {quote(build_msg("Generating docker compose...", False, Color.GREEN))}; \
                                        python ./docker/generate_docker_file.py{" -p" if args.prod else ""};"
        
        starting_db_cmd = f"echo {quote(build_msg("Starting development environment...", False, Color.GREEN))}; \
                            docker compose -f {config.DOCKER_COMPOSE_FOLDER}/docker-compose.yml up -d --build {config.DATABASE_DOCKER_SERVICE_NAME};"
        
        deploy_services_cmd = ""
        for i in range(len(services_name)):
            deploy_services_cmd += f"echo {quote(build_msg(f"Deploying {services_name[i]}...", False, Color.GREEN))}; \
                                     SERVICE_NAME={services_name[i]} docker compose -p {services_name[i]} -f {config.SERVER_DOCKER_COMPOSE_FOLDER}/docker-compose.service.yml up -d --build;"

        starting_other_containers = f"echo {quote(build_msg(f"Starting other containers...", False, Color.GREEN))}; \
                                      docker compose -f {config.DOCKER_COMPOSE_FOLDER}/docker-compose.yml up -d --build {config.FRONT_END_DOCKER_SERVICE_NAME} {config.NGINX_DOCKER_SERVICE_NAME}{"" if args.prod else f" {config.PGADMIN_DOCKER_SERVICE_NAME}"};"

        end_cmd = f"echo {quote(build_msg(f"Deployment complete.", False, Color.GREEN))}; \
                    echo {quote(build_msg(f"Currently running containers:", False, Color.GREEN))}; \
                    docker ps;"
        
        return f"{generate_docker_compose_cmd} \
                 {starting_db_cmd} \
                 {deploy_services_cmd} \
                 {starting_other_containers} \
                 {end_cmd} {f"docker logs -f {str(args.logs)};" if args.logs else ""}"

    def __get_services(self) -> list[str]:
        services_name: list[str] = []
        env_vars = dotenv_values(".env")
        for key in env_vars.keys():
            if key.endswith(config.SERVICE_NAME_SUFFIX):
                services_name.append(env_vars.get(key))
        return services_name