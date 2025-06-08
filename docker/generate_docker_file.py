import json
import yaml
import os
import argparse

NON_ACCESSIBLE_FROM_OUTSIDE_SERVICES = ["database"]
NON_PRODUCTION_SERVICES = ["pgadmin"]
SERVICES_JSON_FILES_PATH = "/json"

GREEN = '\033[0;32m'
RED = '\033[0;31m'
YELLOW = '\033[0;33m'
NC = '\033[0m'

networks_config = { "app-network": { "name": "app-network",
                                     "driver": "bridge" }}

volumes_config = { "postgre": { "name": "postgre",
                                "driver": "local" }}

def generate_docker_compose(services, networks=None, volumes=None, output_file=None):
    docker_compose = { "services": services}
    
    if networks:
        docker_compose["networks"] = networks
        
    if volumes:
        docker_compose["volumes"] = volumes
    
    if output_file:
        with open(output_file, 'w') as file:
            yaml.dump(docker_compose, file, default_flow_style=False)
            print(f"{GREEN}Docker compose file saved to {output_file}{NC}")
    
    return docker_compose

def load_all_services_from_directory(directory_path: str) -> list[dict[str, any]]:
    json_services = []
    
    if not os.path.exists(directory_path):
        print(f"{RED}ERROR: Specified path do not exist !{NC}")
        return []

    for filename in os.listdir(directory_path):
        if filename.endswith('.json'):
            json_services.append(load_service_json(os.path.join(directory_path, filename)))
    
    return json_services

def remove_ports(all_services: dict):
    for service_name in NON_ACCESSIBLE_FROM_OUTSIDE_SERVICES:
        if all_services.get(service_name):
            del all_services[service_name]["ports"]

    return all_services

def load_service_json(file_path) -> dict[str, any]:
    with open(file_path, 'r') as file:
        service_config = json.load(file)
    
    return service_config

def main():
    parser = argparse.ArgumentParser(description="Docker Compose file generator script")
    parser.add_argument("-p", "--prod", action="store_true", help="Generate in production mode.")
    args = parser.parse_args()

    all_services = {}
    current_directory = os.path.dirname(os.path.abspath(__file__))
    service_list = load_all_services_from_directory(current_directory + SERVICES_JSON_FILES_PATH)

    if len(service_list) == 0:
        return print(f"{RED}ERROR: No services found !{NC}")

    for service in service_list:
        service_name = service.get("container_name", None)
        if not service_name:
            return print(f"{RED}ERROR: Service is missing its name. Here is the service file:", "\n", service, f"{NC}")
        
        all_services[service_name] = service

    if args.prod:
        all_services = remove_ports(all_services)
        for service_name in NON_PRODUCTION_SERVICES:
            if not all_services.get(service_name):
                print(f"{YELLOW}WARN: Cannot delete {service_name}, service is missing.{NC}")
                continue
            
            del all_services[service_name]
    generate_docker_compose(all_services, networks_config, volumes_config, f"{current_directory}/docker-compose.yml")
    print(f"{GREEN}All operations completed successfully!\n{NC}")


if __name__ == "__main__":
    main()