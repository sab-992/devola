import os

from .log import log, Color
from .file_system import FileSystem
from settings.config import SERVICES_PATH


class ServiceUpdater():
    def __init__(self):
        self.__fs = FileSystem()

    def update_services(self):
        for service in self.get_services():
            service_path = os.path.join(self.__fs.find_root_folder(), SERVICES_PATH, service)

            if not os.path.exists(service_path):
                Exception("Error while updating services. Service path does not exist!")

            service_name = os.path.basename(service_path)
            libraries = self.__fs.parse_library_file(self.__fs.read(os.path.join(service_path, "settings", "libraries.txt")))

            self.__fs.write(folder_path=service_path, file_name="CMakeLists.txt", content=self.__fs.cmake(os.path.basename(service_path), libraries))
            self.__fs.write(folder_path=f"{service_path}/docker", file_name=f"docker-compose.{service_name}.yml", content=self.__fs.docker_compose(service_name=service_name, env_file_dir=f"{service_path}/settings", libraries=libraries))

        log(f"Updated all services.", False, Color.GREEN)

    def get_services(self) -> list[str]:
        service_full_path = os.path.join(self.__fs.find_root_folder(), SERVICES_PATH)
        os.makedirs(service_full_path, exist_ok=True);
        return self.__fs.get_directories(service_full_path)