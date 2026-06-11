DEPENDENCIES = ["libpq-dev"]

# Libraries
DATABASE_LIB_NAME = "database"

# Paths (always from the root .../devola/)
SERVICES_PATH = "server/service"
EXTRA_BUILD_OPTIONS_FILENAME = "extra_build_options.json"
ROOT_FOLDER_NAME = "devola-unrefactored"

# Docker containers names
DATABASE_DOCKER_SERVICE_NAME  = "database"
FRONT_END_DOCKER_SERVICE_NAME = "angular"
NGINX_DOCKER_SERVICE_NAME     = "nginx"
PGADMIN_DOCKER_SERVICE_NAME   = "pgadmin"

# Windows only:
POSTGRE_INSTALLATION_PATH = "C:/Program Files/PostgreSQL/17"
USE_VCPKG = False

# Linux only:
UPDATE_PACKAGE_REPOS_COMMAND = "sudo pacman -Syu"
INSTALL_COMMAND = "sudo pacman -S"
LIBPQXX_PACKAGE = "postgresql"