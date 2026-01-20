# TODO: Make sure this is up to date
DEPENDENCIES = ["libpq-dev", "zlib"]

# Libraries
DATABASE_LIB_NAME = "database"

# Paths (always from the root .../devola/)
DEBUG_EXECUTABLE_PATH_FROM_BUILD_DIR = "server/dev_server"
SERVICES_PATH = "server/service"
ROOT_FOLDER_NAME = "devola"

# Docker containers names
DATABASE_DOCKER_SERVICE_NAME  = "database"
FRONT_END_DOCKER_SERVICE_NAME = "angular"
NGINX_DOCKER_SERVICE_NAME     = "nginx"
PGADMIN_DOCKER_SERVICE_NAME   = "pgadmin"

# Windows only:
POSTGRE_INSTALLATION_PATH = "C:/Program Files/PostgreSQL/17"
USE_VCPKG = False