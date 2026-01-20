import os
from pathlib import Path

from .config import ROOT_FOLDER_NAME, DATABASE_LIB_NAME
from .errors import InvalidInputError
from .log import log, Color

class FileSystem():
    def __init__(self):
        pass

    def cmake(self, service_name: str, libraries: set[str]) -> str:
        libraries_sorted = sorted(libraries)
        service_name_lower = service_name.lower()

        librairies_str = "core_lib"
        for lib in libraries_sorted:
            librairies_str += f" {lib.lower() + "_lib"}"
        return f"""\
cmake_minimum_required(VERSION 3.16)

set(cmake_modules_path "${'{'}CMAKE_CURRENT_LIST_DIR{'}'}/../../../cmake")
set({service_name_lower}_path "${'{'}CMAKE_CURRENT_LIST_DIR{'}'}")

include("${'{'}cmake_modules_path{'}'}/FindCppFiles.cmake")
{f"""include("${'{'}cmake_modules_path{'}'}/DatabaseLibrary.cmake")""" if DATABASE_LIB_NAME in libraries else ""}

project({service_name_lower} VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_cpp_files("${'{'}{service_name_lower}_path{'}'}/src" {service_name_lower}_sources)

add_executable ({service_name_lower} "${'{'}{service_name_lower}_path{'}'}/main.cpp" ${'{'}{service_name_lower}_sources{'}'})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET {service_name_lower} PROPERTY CXX_STANDARD 20)
endif()

target_include_directories({service_name_lower} PRIVATE "${'{'}{service_name_lower}_path{'}'}/include")
target_link_libraries({service_name_lower} PRIVATE {librairies_str})
"""

    def cpp(self, service_name: str) -> str:
        return f"""\
#include <iostream>


int main() {'{'}
    std::cout << "Hello world from {service_name.lower()} !" << std::endl;
    return 0;
{'}'}
"""

    def docker_compose(self, service_name: str, env_file_dir: str, libraries: set[str]) -> str:
        path_to_server = "../../.."
        path_to_root_folder = f"{path_to_server}/.."
        lib_volumes: str = ""
        libraries_sorted = sorted(libraries)
        for lib in libraries_sorted:
            lib_volumes += f"      - {path_to_root_folder}/lib/{lib}:/app/lib/{lib}\n"
        return f"""\
services:
  microservice:
    image: gcc:latest
    working_dir: /app
    env_file:
      - {env_file_dir}/{service_name}.env
    container_name: {service_name}
    environment:
      - SERVICE_NAME={service_name}
    networks:
      - app-network
    volumes:
      - ../../{service_name}:/app/server/service/{service_name}
      - {path_to_server}/CMakeLists.txt:/app/server/CMakeLists.txt
      - {path_to_root_folder}/cmake:/app/cmake
      - {path_to_root_folder}/CMakeLists.txt:/app/CMakeLists.txt
      - {path_to_root_folder}/lib/CMakeLists.txt:/app/lib/CMakeLists.txt
      - {path_to_root_folder}/lib/core:/app/lib/core
{lib_volumes}
    restart: unless-stopped
    command:
      - bash
      - -c
      - |
        apt-get update;
        apt-get install -y git cmake build-essential pkg-config libssl-dev libsasl2-dev {"libpq-dev" if DATABASE_LIB_NAME in libraries else ""};
        rm -rf build/
        mkdir -p build;
        cd build;
        cmake -DCMAKE_BUILD_TYPE=release -DSERVICE_NAME={service_name} ..;
        cmake --build . --parallel $$(nproc);
        ./server/{service_name}
networks:
  app-network:
    external: true
"""

    def find_root_folder(self):
        current_path = Path.cwd()

        for parent in [current_path] + list(current_path.parents):
            current_path = parent / ROOT_FOLDER_NAME
            if current_path.exists():
                return current_path
            
        raise Exception("Root folder not found!")

    def git_ignore(self, name: str) -> str:
        return f"""\
/docker/docker-compose.{name}.yml
"""

    def get_directories(self, path: str) -> list[str]:
        return [directory for directory in os.listdir(path) if os.path.isdir(os.path.join(path, directory))]

    def make_directory(self, dir_path: str, dir_name: str):
        root_folder = self.find_root_folder()
        new_service_directory_path = os.path.join(root_folder, dir_path, dir_name)
        os.makedirs(new_service_directory_path, exist_ok=True)
        log(f"Created: \"{dir_name}\" directory ", True, Color.GREEN)
        return new_service_directory_path

    def nginx(self, service_name: str, use_upload_config: bool = False) -> str:
        return f"""\
server {'{'}
    location /api/{service_name} {'{'}
{
f"""\
        proxy_pass http://{service_name};
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;

        # Upload specific settings
        proxy_request_buffering off;
        proxy_buffering off;
        proxy_http_version 1.1;

        chunked_transfer_encoding on;
""" if use_upload_config else f"""\
    location /api/{service_name} {'{'}
        proxy_pass http://{service_name};
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_cache_bypass $http_upgrade;"
"""
}
    {'}'}
{'}'}
"""
    def packages(self, libraries: set[str]) -> str:
        find_packages = "find_package(core_lib REQUIRED)"
        libraries_sorted = sorted(libraries)
        for lib in libraries_sorted:
            find_packages += f"\nfind_package({lib.lower()}_lib REQUIRED)"
        return find_packages

    def parse_library_file(self, file_content: str) -> set[str]:
        return {lib.strip() for lib in file_content.strip().splitlines() if lib.strip()}

    def read(self, file_path: str) -> str:
        if len(file_path) <= 0:
            raise InvalidInputError("Empty path!")
        if not os.path.isfile(file_path):
            raise InvalidInputError("Path does not exists!")

        with open(file_path, 'r') as f:
            return f.read()

    def write(self, folder_path: str, file_name: str, content: str, skip_if_exists: bool=False):
        if len(folder_path) <= 0:
            raise InvalidInputError("Empty path!")
        if len(file_name) <= 0:
            raise InvalidInputError("Empty file name!")

        os.makedirs(folder_path, exist_ok=True)
        file_path = os.path.join(folder_path, file_name)

        if (os.path.isfile(file_path) and skip_if_exists):
            return

        operation: str = "Updated" if os.path.isfile(file_path) else "Created"

        with open(file_path, 'w') as f:
            f.write(content)

        log(f"{operation}: \"{file_name}\"", True, Color.YELLOW)