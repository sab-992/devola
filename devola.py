import os
from dotenv import dotenv_values
import subprocess
import argparse

DOTENV_PATH = "./"
SERVICE_NAME_SUFFIX = "_SERVICE"

DATABASE_DOCKER_SERVICE_NAME = "database"
FRONT_END_DOCKER_SERVICE_NAME = "angular"
NGINX_DOCKER_SERVICE_NAME = "nginx"
PGADMIN_DOCKER_SERVICE_NAME = "pgadmin"

DATABASE_VOLUME_NAME = "postgre"

DOCKER_COMPOSE_FOLDER = "./docker"
SERVER_DOCKER_COMPOSE_FOLDER = "./server/docker"

SERVICES_PATH = "/server/src/"

# Colors
GREEN = '\033[0;32m'
RED = '\033[0;31m'
YELLOW = '\033[0;33m'
NC = '\033[0m'

def main():
    parser = argparse.ArgumentParser(description="Server deployment and stop script.")
    parser.add_argument("operation", help="Operation <make | cmake | makeall | deploy | stop | test | add_service>")

    parser.add_argument("-s", "--start", action="store_true", help="(For 'make' and 'makeall' only) Starts the application locally.")
    parser.add_argument("-p", "--prod", action="store_true", help="(For 'deploy' only) Deploy in production mode.")
    parser.add_argument("-c", "--clean", action="store_true", help="(For 'deploy' only) Removes EVERYTHING about Docker (for development and test modes ONLY), \
                                                                    it also removes the database for every mode except 'Production'.")

    parser.add_argument("-r", "--regex", type=str, help="(For 'tests' only) To launch specific tests")
    parser.add_argument("-l", "--logs", type=str, help="(For 'deploy' only) Display logs for the chosen docker")
    
    parser.add_argument("-sp", "--service_path", type=str, help="(For 'add_service' only) Path of the created service.")
    parser.add_argument("-n", "--name", type=str, help="(For 'add_service' only) Name of the created service.")

    args = parser.parse_args()

    match args.operation:
        case "add_service":
            if not args.name or len(args.name) == 0:
                log(f"Missing name of service to create !", False, RED)
                return
            
            create_service(args.name, args.service_path)
            return
        case "make":
            make(args)
        case "cmake":
            cmake()
        case "makeall":
            makeall(args)
        case "deploy":
            deploy(get_services(), args)
        case "stop":
            stop(args)
        case "stop":
            stop(args)
        case "test":
            test(args)

    if args.logs:
        subprocess.run(f"docker logs -f {str(args.logs)}", shell=True)

def make(args: argparse.Namespace):
    try:
        env = os.environ.copy()
        subprocess.run(f"cd server; cd build; cmake --build . --parallel $(nproc); {"./devola; " if args.start else ""}cd ../..", shell=True, env=env)
    except Exception as e:
        log(f"Something went wrong while compiling: {e}", False, RED)

def cmake():
    try:
        env = os.environ.copy()
        subprocess.run(f"cd server; mkdir -p build; cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..; cd ../..", shell=True, env=env)
    except Exception as e:
        log(f"Something went wrong while compiling: {e}", False, RED)

def makeall(args: argparse.Namespace):
    try:
        env = os.environ.copy()
        subprocess.run(f"cd server; mkdir -p build; cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..; cmake --build . --parallel $(nproc); {"./devola; " if args.start else ""}cd ../..", shell=True, env=env)
    except Exception as e:
        log(f"Something went wrong while compiling: {e}", False, RED)

def deploy(services_name: list[str], args: argparse.Namespace):
    try:
        env = os.environ.copy()
        log(f"Generating docker compose...", False, GREEN)
        subprocess.run(f"python ./docker/generate_docker_file.py{" -p" if args.prod else ""}", shell=True)

        log(f"Starting development environment...", False, GREEN)
        subprocess.run(f"docker compose -f {DOCKER_COMPOSE_FOLDER}/docker-compose.yml up -d --build {DATABASE_DOCKER_SERVICE_NAME}", shell=True)

        for i in range(len(services_name)):
            log(f"Deploying {services_name[i]}...", False, GREEN)
            env["SERVICE_NAME"] = services_name[i]
            subprocess.run(f"SERVICE_NAME={services_name[i]} docker compose -p {services_name[i]} -f {SERVER_DOCKER_COMPOSE_FOLDER}/docker-compose.service.yml up -d --build", shell=True, env=env)

        log(f"Starting other containers...", False, GREEN)
        subprocess.run(f"docker compose -f {DOCKER_COMPOSE_FOLDER}/docker-compose.yml up -d --build {FRONT_END_DOCKER_SERVICE_NAME} {NGINX_DOCKER_SERVICE_NAME}{"" if args.prod else f" {PGADMIN_DOCKER_SERVICE_NAME}"}", shell=True)
        
        log(f"Deployment complete.", False, GREEN)
        log(f"Currently running containers:", False, GREEN)
        subprocess.run("docker ps", shell=True)
    except Exception as e:
        log(f"Something went wrong while deploying: {e}", False, RED)

def stop(args: argparse.Namespace):
    try:
        if args.clean:
            subprocess.run(f"docker rm -f $(docker ps -aq) 2>/dev/null; docker rmi -f $(docker images -aq) 2>/dev/null; docker network prune -f; docker volume ls -q | grep -v \"{DATABASE_VOLUME_NAME}\" | xargs -r docker volume rm; docker volume rm {DATABASE_VOLUME_NAME};", shell=True)
        else:
            subprocess.run("docker rm -f $(docker ps -aq)", shell=True)
            subprocess.run("docker rmi -f $(docker images -q)", shell=True)
            subprocess.run("docker network rm app-network", shell=True)

        log(f"Cleaning complete", False, GREEN)
    except Exception as e:
        log(f"Something went wrong while stopping: {e}", False, RED)

def test(args: argparse.Namespace):
    try:
        env = os.environ.copy()
        subprocess.run(f"cd server; mkdir -p build; cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..; cmake --build . --parallel $(nproc); ctest {f"-R \"{args.regex}\"" if args.regex else ""}; cd ../..", shell=True, env=env)
    except Exception as e:
        log(f"Something went wrong while stopping: {e}", False, RED)

# --------------------------------------------------------------------------------------
# -                                                                                    -
# - HELPER FUNCTIONS                                                                   -
# -                                                                                    -
# --------------------------------------------------------------------------------------

def get_services() -> list[str]:
    services_name: list[str] = []
    env_vars = dotenv_values(".env")
    for key in env_vars.keys():
        if key.endswith(SERVICE_NAME_SUFFIX):
            services_name.append(env_vars.get(key))
    return services_name

def create_service(name: str, path):
    if not path or len(path) == 0:
        path = SERVICES_PATH

    current_directory = os.path.dirname(os.path.abspath(__file__))
    new_service_directory_path = f"{current_directory}{path}{name}"

    # Service Folder
    os.makedirs(new_service_directory_path, exist_ok=True)
    
    # main.cpp
    if not generate_main_file(new_service_directory_path, name):
        return
    
    # server/<service_name>/CMakeLists.txt
    if not generate_cmake_file(new_service_directory_path, name):
        return

def generate_main_file(path, service_name):
    try:
        os.makedirs(path, exist_ok=True)
        
        file_path = os.path.join(path, "main.cpp")
        
        # Create the content
        content = \
f"""\
#include <iostream>


int main() {'{'}
    std::cout << "Hello world from {service_name.lower()} !" << std::endl;
    return 0;
{'}'}
"""
        with open(file_path, 'w') as f:
            f.write(content)
        
        log(f"\"main.cpp\" created at: {file_path}", True, GREEN)
        return True
    except Exception as e:
        log(f"Something went wrong while generating the service's \"main.cpp\" file: {e}", True, RED)
        return False

def generate_cmake_file(path, service_name):
    try:
        os.makedirs(path, exist_ok=True)
        
        file_path = os.path.join(path, "CMakeLists.txt")
        service_name_lower = service_name.lower()
        content = \
f"""\
cmake_minimum_required(VERSION 3.16)

project({service_name_lower} VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set({service_name.upper()}_PATH "${'{'}CMAKE_CURRENT_LIST_DIR{'}'}")

add_executable ({service_name_lower} "${'{'}{service_name.upper()}_PATH{'}'}/main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET {service_name_lower} PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries({service_name_lower} PRIVATE CORE)
target_include_directories({service_name_lower} PRIVATE ${'{'}CORE_INCLUDES{'}'})
"""
        with open(file_path, 'w') as f:
            f.write(content)

        log(f"\"CMakeLists.txt\" created at: {file_path}", True, GREEN)
        return True
    except Exception as e:
        log(f"Something went wrong while generating the service's \"CMakeLists.txt\" file: {e}", True, RED)
        return False

def log(message: str, ressource_creation: bool, mode=GREEN):
    if ressource_creation:
        print(f"{mode}{"[+]"if mode != RED else "[x]"}{NC} {message}")
    else:
        print(f"{mode}{message}{NC}")

if __name__ == "__main__":
    main()