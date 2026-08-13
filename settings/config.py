DEPENDENCIES: list[str] = ["libpq-dev"]

# Libraries
DATABASE_LIB_NAME: str = "database"

# Paths (always from the root .../devola/)
SERVICES_PATH: str = "server/service"
EXTRA_BUILD_OPTIONS_FILENAME: str = "extra_build_options.json"
ROOT_FOLDER_NAME: str = "devola-unrefactored"

# Docker containers names
FRONT_END_DOCKER_SERVICE_NAME: str = "angular"
NGINX_DOCKER_SERVICE_NAME: str     = "nginx"
PGADMIN_DOCKER_SERVICE_NAME: str   = "pgadmin"

# Windows only:
POSTGRE_INSTALLATION_PATH: str = "C:/Program Files/PostgreSQL/17"
USE_VCPKG: bool = False

# Linux only:
UPDATE_PACKAGE_REPOS_COMMAND: str = "sudo pacman -Syu"
INSTALL_COMMAND: str = "sudo pacman -S"
LIBPQXX_PACKAGE: str = "postgresql"