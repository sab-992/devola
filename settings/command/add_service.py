from argparse import Namespace

from settings.config import SERVICES_PATH
from settings.command.detail.command import Command
from settings.command.detail.file_system import FileSystem
from settings.command.detail.log import log, Color
from settings.command.detail.options import SERVICE_NAME_OPTION, SERVICE_PATH_OPTION, MANUAL_OPTION


class AddService(Command):
    def __init__(self):
        Command.__init__(self)
        self.__fs = FileSystem()

    def arguments(self) -> dict[str, dict]:
        return { "name": SERVICE_NAME_OPTION, "service_path": SERVICE_PATH_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "add_service"

    def command_explicit(self, args: Namespace) -> list[list[str]]:
        self.__add_service(args.name, args.service_path)
        return [] # We return no command.

    def details(self) -> str:
        return "Add a service and all the necessary start files."

    def setup(self, args: Namespace) -> str:
        pass

    def teardown(self, args: Namespace) -> str:
        pass

    def __add_service(self, name: str, path):
        try:
            if not path or len(path) == 0:
                path = SERVICES_PATH

            # Service Folder
            new_service_directory_path = self.__fs.make_directory(path, name)

            # TODO: Add new service nginx file.
            libraries: set[str] = set()

            self.__fs.make_directory(new_service_directory_path, "src")
            self.__fs.make_directory(new_service_directory_path, "test")
            self.__fs.make_directory(new_service_directory_path, "include")
            docker_dir = self.__fs.make_directory(new_service_directory_path, "docker")
            settings_dir = self.__fs.make_directory(new_service_directory_path, "settings")

            self.__fs.write(new_service_directory_path, file_name=f".gitignore", content=self.__fs.git_ignore(name), skip_if_exists=True)
            self.__fs.write(new_service_directory_path, file_name="main.cpp", content=self.__fs.cpp(name), skip_if_exists=True)
            self.__fs.write(new_service_directory_path, file_name="CMakeLists.txt", content=self.__fs.cmake(name, libraries))

            self.__fs.write(folder_path=settings_dir, file_name=f"{name}.env", content="", skip_if_exists=True)
            self.__fs.write(folder_path=settings_dir, file_name="libraries.txt", content="", skip_if_exists=True)

            self.__fs.write(folder_path=docker_dir, file_name=f".gitkeep", content="")
            self.__fs.write(folder_path=docker_dir, file_name=f"docker-compose.{name}.yml", content=self.__fs.docker_compose(service_name=name, env_file_dir=settings_dir, libraries=libraries))

            log(f"\"{name}\" service has been created successfully. Location: {new_service_directory_path}", False, Color.GREEN)
        except Exception as e:
            log(f"Something went wrong while creating the new service: {e}", True, Color.RED)