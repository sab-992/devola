import json
import yaml
import os
import argparse

NON_PRODUCTION_SERVICES = ["pgadmin"]
SERVICES_JSON_FILES_DIR = "/json"

GREEN = '\033[0;32m'
RED = '\033[0;31m'
YELLOW = '\033[0;33m'
NC = '\033[0m'

networks_config = { "app-network": { "name": "app-network",
                                     "driver": "bridge" }}

volumes_config = {}

def generate_docker_compose(services, networks=None, volumes=None, output_file=None):
    docker_compose = { "services": services}

    if networks and len(networks) > 0:
        docker_compose["networks"] = networks

    if volumes and len(volumes) > 0:
        docker_compose["volumes"] = volumes

    if output_file:
        with open(output_file, 'w') as file:
            yaml.dump(docker_compose, file, default_flow_style=False)
            print(f"{GREEN}Docker compose file saved to {output_file}{NC}")

    return docker_compose

def load_services(directory_path: str) -> list[dict[str, any]]:
    json_services = []

    if not os.path.exists(directory_path):
        print(f"{RED}ERROR: Specified path do not exist !{NC}")
        return []

    for filename in os.listdir(directory_path):
        if filename.endswith('.json'):
            json_services.append(load_json(os.path.join(directory_path, filename)))

    return json_services

def load_json(file_path) -> dict[str, any]:
    with open(file_path, 'r') as file:
        service_config = json.load(file)
    return service_config

def remove_non_production_services(services: list):
    for service_name in NON_PRODUCTION_SERVICES:
        if not services.get(service_name):
            print(f"{YELLOW}WARN: Cannot delete {service_name}, service is missing.{NC}")
            continue

        del services[service_name]
    return services

def get_services(docker_dir: str):
    services = {}

    for service in load_services(docker_dir + SERVICES_JSON_FILES_DIR):
        service_name = service.get("container_name", None)
        if not service_name:
            return print(f"{RED}ERROR: Service is missing its name. Here is the service file:", "\n", service, f"{NC}")

        services[service_name] = service

    return services

def main():
    parser = argparse.ArgumentParser(description="Docker Compose file generator script")
    parser.add_argument("-p", "--prod", action="store_true", help="Generate in production mode.")
    args = parser.parse_args()

    docker_directory = os.path.dirname(os.path.abspath(__file__))

    services = get_services(docker_directory)

    if len(services) == 0:
        return print(f"{RED}ERROR: No services found !{NC}")

    if args.prod:
        services = remove_non_production_services(services)

    generate_docker_compose(services=services,
                            networks=networks_config,
                            volumes=volumes_config,
                            output_file=f"{docker_directory}/docker-compose.yml")

if __name__ == "__main__":
    main()