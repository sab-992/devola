import os
from dotenv import dotenv_values
import subprocess
import argparse
from pathlib import Path

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

def main():
    parser = argparse.ArgumentParser(description="Server deployment and stop script.")
    parser.add_argument("operation", help="Operation <deploy | stop | service>")

    parser.add_argument("-p", "--prod", action="store_true", help="Deploy in production mode.")
    parser.add_argument("-c", "--clean", action="store_true", help="Removes EVERYTHING about Docker (for development and test modes ONLY), \
                                                                    it also removes the database for every mode except 'Production'.")
    parser.add_argument("-l", "--logs", type=str, help="Display logs for the chosen docker")
    parser.add_argument("-sn", "--service_name", type=str, help="Name of the created service.")
    parser.add_argument("-sp", "--service_path", type=str, help="Path of the created service.")
    args = parser.parse_args()

    if args.operation == "service":
        if not args.service_name or len(args.service_name) == 0:
            print("ERROR: Missing name of service to create !")
            return
        
        if not args.service_path or len(args.service_path) == 0:
            create_service(args.service_name)
        else:
            create_service(args.service_name, args.service_path)
        
        return

    if args.operation == "deploy":
        deploy(get_services(), args)
    elif args.operation == "stop":
        stop(args)
    if args.logs:
        subprocess.Popen(f"docker logs -f {str(args.logs)}", shell=True)

def get_services() -> list[str]:
    services_name: list[str] = []
    env_vars = dotenv_values(".env")
    for key in env_vars.keys():
        if key.endswith(SERVICE_NAME_SUFFIX):
            services_name.append(env_vars.get(key))
    return services_name

def deploy(services_name: list[str], args: argparse.Namespace):
    try:
        subprocess.run("echo \"Generating docker compose...\"", shell=True)
        subprocess.run(f"python ./docker/generate_docker_file.py{" -p" if args.prod else ""}", shell=True)

        subprocess.run("echo \"Starting development environment...\"", shell=True)
        subprocess.run(f"docker compose -f {DOCKER_COMPOSE_FOLDER}/docker-compose.yml up -d --build {DATABASE_DOCKER_SERVICE_NAME}", shell=True)

        for i in range(len(services_name)):
            print(services_name[i])
            subprocess.run(f"echo \"Deploying backend services: {services_name[i]} ...\"", shell=True)
            env = os.environ.copy()
            env["SERVICE_NAME"] = services_name[i]
            subprocess.run(f"SERVICE_NAME={services_name[i]} BUILD_TYPE={"Release" if args.prod else "Debug"} docker compose -p {services_name[i]} -f {SERVER_DOCKER_COMPOSE_FOLDER}/docker-compose.service.yml up -d --build", shell=True, env=env)

        subprocess.run('echo "Starting other containers..."', shell=True)
        subprocess.run(f"docker compose -f {DOCKER_COMPOSE_FOLDER}/docker-compose.yml up -d --build {FRONT_END_DOCKER_SERVICE_NAME} {NGINX_DOCKER_SERVICE_NAME}{"" if args.prod else f" {PGADMIN_DOCKER_SERVICE_NAME}"}", shell=True)
        subprocess.run("echo \"Deployment completed.\"", shell=True)
        subprocess.run("echo \"Currently running containers:\"", shell=True)
        subprocess.run("docker ps", shell=True)
    except Exception as e:
        print("Something went wrong while deploying in development mode: ", e)

def stop(args: argparse.Namespace):
    try:
        if args.clean:
            subprocess.run(f"docker rm -f $(docker ps -aq) 2>/dev/null; docker rmi -f $(docker images -aq) 2>/dev/null; docker network prune -f; docker volume ls -q | grep -v \"{DATABASE_VOLUME_NAME}\" | xargs -r docker volume rm; docker volume rm {DATABASE_VOLUME_NAME};", shell=True)
        else:
            subprocess.run("docker rm -f $(docker ps -aq)", shell=True)
            subprocess.run("docker rmi -f $(docker images -q)", shell=True)
            subprocess.run("docker network rm app-network", shell=True)
    except Exception as e:
        print("Something went wrong while stopping: ", e)

def create_service(name: str, path=SERVICES_PATH):
    current_directory = os.path.dirname(os.path.abspath(__file__))
    new_service_directory_path = f"{current_directory}{path}{name}"

    # Service Folder
    os.makedirs(new_service_directory_path, exist_ok=True)
    
    # main.cpp
    if not generate_main_file(new_service_directory_path, name):
        return
    
    # CMakeLists.txt
    if not generate_cmake_file(new_service_directory_path, name):
        return

def generate_main_file(path, service_name):
    os.makedirs(path, exist_ok=True)
    
    file_path = os.path.join(path, "main.cpp")
    
    # Create the content
    content = \
f"""
#include <iostream>


int main() {'{'}
    std::cout << "Hello world !" << std::endl;
    return 0;
{'}'}
"""
    
    with open(file_path, 'w') as f:
        f.write(content)
    
    print(f"main.cpp created at: {file_path}")
    return True

def generate_cmake_file(path, service_name):
    os.makedirs(path, exist_ok=True)
    
    file_path = os.path.join(path, "CMakeLists.txt")
    service_name_lower = service_name.lower()
    content = \
f"""
cmake_minimum_required (VERSION 3.10)

project({service_name_lower} VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set({service_name.upper()}_DIR "${'{'}PROJECT_SOURCE_DIR{'}'}/src/{service_name_lower}")
set(LIB_DIR "${'{'}PROJECT_SOURCE_DIR{'}'}/lib")
set(CORE_DIR "${'{'}LIB_DIR{'}'}/core")

add_executable ({service_name_lower} "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET {service_name_lower} PROPERTY CXX_STANDARD 20)
endif()

target_include_directories({service_name_lower} PRIVATE ${'{'}CORE_DIR{'}'})
"""

    with open(file_path, 'w') as f:
        f.write(content)
    
    print(f"CMakeLists.txt created at: {file_path}")
    return True

if __name__ == "__main__":
    main()