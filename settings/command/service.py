from argparse import Namespace

from settings.config import SERVICES_PATH
from settings.command.detail.command import Command
from settings.command.detail.file_system import FileSystem
from settings.command.detail.log import log, Color
from settings.command.detail.options import SERVICE_ADD_OPTION, SERVICE_DATABASE_OPTION, SERVICE_DATABASE_IDENTIFIER_OPTION, SERVICE_NAME_OPTION, SERVICE_PATH_OPTION, MANUAL_OPTION


class Service(Command):
    def __init__(self):
        Command.__init__(self)
        self.__fs = FileSystem()

    def arguments(self) -> dict[str, dict]:
        return { "add": SERVICE_ADD_OPTION, "database": SERVICE_DATABASE_OPTION, "identifier": SERVICE_DATABASE_IDENTIFIER_OPTION, "name": SERVICE_NAME_OPTION, "service_path": SERVICE_PATH_OPTION, "manual": MANUAL_OPTION }

    def command(self) -> str:
        return "service"

    def command_explicit(self, args: Namespace) -> list[list[str]]:
        if self.is_parameter_empty(args.name):
            raise ValueError("No name for the new service was specified")

        if self.is_parameter_empty(args.database):
            raise ValueError("No database was specified")
        elif self.is_parameter_empty(args.identifier):
            raise ValueError("No database identifier was specified")

        if args.add:
            self.__add_service(args)

        if args.database:
            self.__add_database(args)

        return [] # We return no command.

    def details(self) -> str:
        return "Add a service and all the necessary start files."

    def setup(self, args: Namespace) -> str:
        pass

    def teardown(self, args: Namespace) -> str:
        pass

    def __add_database(self, args: Namespace):
        name: str = args.name
        path: str = args.service_path

        db: str = args.database
        db_id: str = args.identifier

        if not path or len(path) <= 0:
            path = SERVICES_PATH
        else:
            path.replace("\"", " ")

        target_service_directory = f"{path}/{name}"

        database_dir = self.__fs.make_directory(target_service_directory, "database")
        database_files_dir = self.__fs.make_directory(database_dir, db_id)

        # If skipped writing, we send a warning message
        if (self.__fs.write(folder_path=f"{target_service_directory}/docker", file_name=f"docker-compose.database.{db_id}.yml", content=self.__fs.database_compose(db, name, db_id), skip_if_exists=True)):
            log(f"Database file already exists, skipping...", False, Color.YELLOW)

        full_service_path = f"{self.__fs.find_root_folder()}/{target_service_directory}"
        log(f"\"{db_id}\" database has been created successfully. Location: '{full_service_path}/docker'", False, Color.GREEN)
        log(f"Configure your database connection details in '{full_service_path}/settings/.env'.", False, Color.YELLOW)

    def __add_service(self, args: Namespace):
        try:
            name: str = args.name
            path: str = args.service_path

            if not path or len(path) <= 0:
                path = SERVICES_PATH
            else:
                path.replace("\"", " ")

            # Service Folder
            new_service_directory_path = self.__fs.make_directory(path, name)
            root_folder = self.__fs.find_root_folder()

            libraries: set[str] = set()

            self.__fs.make_directory(new_service_directory_path, "src")
            self.__fs.make_directory(new_service_directory_path, "test")
            self.__fs.make_directory(new_service_directory_path, "include")
            docker_dir = self.__fs.make_directory(new_service_directory_path, "docker")
            settings_dir = self.__fs.make_directory(new_service_directory_path, "settings")

            self.__update_default_nginx_file(name, root_folder)
            self.__fs.write(folder_path=f"{root_folder}/nginx/conf.d", file_name=f"{name}.conf", content=self.__fs.nginx(name))

            self.__fs.write(new_service_directory_path, file_name=f".gitignore", content=self.__fs.git_ignore(name), skip_if_exists=True)
            self.__fs.write(new_service_directory_path, file_name="main.cpp", content=self.__fs.cpp(name), skip_if_exists=True)
            self.__fs.write(new_service_directory_path, file_name="CMakeLists.txt", content=self.__fs.cmake(name, libraries))

            self.__fs.write(folder_path=settings_dir, file_name=f".env", content="", skip_if_exists=True)
            self.__fs.write(folder_path=settings_dir, file_name="libraries.txt", content="", skip_if_exists=True)

            self.__fs.write(folder_path=docker_dir, file_name=f".gitkeep", content="")
            self.__fs.write(folder_path=docker_dir, file_name=f"docker-compose.{name}.yml", content=self.__fs.docker_compose(service_name=name, env_file_dir=settings_dir, libraries=libraries))

            log(f"\"{name}\" service has been created successfully. Location: '{new_service_directory_path}'", False, Color.GREEN)
        except Exception as e:
            log(f"Something went wrong while creating the new service: {e}", True, Color.RED)

    def __update_default_nginx_file(self, service_name: str, root_folder: str):
        nginx_dir = f"{root_folder}/nginx"
        content = self.__fs.read(f"{nginx_dir}/nginx.conf")

        content = content.rsplit('    }\n}')[0]
        content += "\n        include /etc/nginx/conf.d/*.conf;\n    }\n}"

        self.__fs.write(folder_path=nginx_dir, file_name=f"nginx.conf", content=content)

    def is_parameter_empty(self, param):
        return param and len(param) <= 0