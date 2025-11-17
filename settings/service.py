import os
from argparse import Namespace
from settings.helper.command import Command
from settings.helper.log import log, Color
from settings.helper.config import SERVICES_PATH
from settings.helper.options import NAME_OPTION, SERVICE_PATH_OPTION, MANUAL_OPTION

class AddService(Command):
    def __init__(self):
        pass

    def arguments(self) -> dict[str, dict]:
        return { "name": NAME_OPTION, "service_path": SERVICE_PATH_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "add_service"

    def command_explicit(self, args: Namespace) -> str:
        self.__add_service(args.name, args.service_path)
        return "" # We return no command.
    
    def details(self) -> str:
        return "Add a service and all the necessary start files."

    def __add_service(self, name: str, path):
        if not path or len(path) == 0:
            path = SERVICES_PATH

        current_directory = os.path.dirname(os.path.abspath(__file__))
        new_service_directory_path = f"{current_directory}{path}{name}"

        # Service Folder
        os.makedirs(new_service_directory_path, exist_ok=True)
        
        # main.cpp
        if not self.__generate_main_file(new_service_directory_path, name):
            return
        
        # server/<service_name>/CMakeLists.txt
        if not self.__generate_cmake_file(new_service_directory_path, name):
            return

    def __generate_main_file(self, path, service_name):
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
            
            log(f"\"main.cpp\" created at: {file_path}", True, Color.GREEN)
            return True
        except Exception as e:
            log(f"Something went wrong while generating the service's \"main.cpp\" file: {e}", True, Color.RED)
            return False

    def __generate_cmake_file(self, path, service_name):
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

find_files(${service_name.upper()}_SOURCES ${service_name.upper()}_PATH "cpp")
add_executable ({service_name_lower} ${'{'}{service_name.upper()}_SOURCES{'}'})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET {service_name_lower} PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries({service_name_lower} PRIVATE CORE)

find_includes({service_name.upper()}_INCLUDES ${'{'}{service_name.upper()}_PATH{'}'})
target_include_directories({service_name_lower} PRIVATE ${'{'}CORE_INCLUDES{'}'} ${'{'}{service_name.upper()}_INCLUDES{'}'})
"""
            with open(file_path, 'w') as f:
                f.write(content)

            log(f"\"CMakeLists.txt\" created at: {file_path}", True, Color.GREEN)
            return True
        except Exception as e:
            log(f"Something went wrong while generating the service's \"CMakeLists.txt\" file: {e}", True, Color.RED)
            return False