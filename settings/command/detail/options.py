from enum import Enum

from settings.config import ROOT_FOLDER_NAME


class OptionsType(Enum):
    ACTION = 0
    TYPE = 1

CLEAN_OPTION                       = { "type": OptionsType.ACTION, "value": "store_true", "required": False, "help":  "(Optional) Removes EVERYTHING about Docker (for development and test modes ONLY), it also removes the database for every mode except 'Production'." }
LAUNCH_OPTION                      = { "type": OptionsType.ACTION, "value": "store_true", "required": False, "help":  "(Optional) Starts the application in debug mode." }
MANUAL_OPTION                      = { "type": OptionsType.ACTION, "value": "store_true", "required": False, "help":  "(Optional) Describes and explain a specific command." }
OUTPUT_OPTION                      = { "type": OptionsType.TYPE,   "value": str,          "required": False, "help":  "(Optional) Display output for the given docker at the end of the deployment. Example: 'python devola.py deploy -o nginx'" }
PROD_OPTION                        = { "type": OptionsType.ACTION, "value": "store_true", "required": False, "help":  "(Optional) Deploy in production mode." }
REGEX_OPTION                       = { "type": OptionsType.TYPE,   "value": str,          "required": False, "help":  "(Optional) Launches tests matching the given regex sequence. Example: 'python devola.py test -r \"*Regex*\"'" }
SERVICE_ADD_OPTION                 = { "type": OptionsType.ACTION, "value": "store_true", "required": False, "help":  "(Optional) Creates a new service with the given name. Example: 'python devola.py service -a -n user_service\"'" }
SERVICE_DATABASE_OPTION            = { "type": OptionsType.TYPE,   "value": str,          "required": False, "help": f"(Optional) Adds docker-compose file of given database type for the service. The command skips if the file exist already. Example: 'python devola.py service -n target_service -d pg_sql -i example_pg_sql'. This example requires the pg_sql.Dockerfile (used for the container-specific command) file to exist. in \".../{ROOT_FOLDER_NAME}/server/docker\"" }
SERVICE_DATABASE_IDENTIFIER_OPTION = { "type": OptionsType.TYPE,   "value": str,          "required": False, "help": f"(Required only if --database option is chosen) Gives a unique identifier to the database. The identifier NEEDS to be unique. Example: 'python devola.py service -n target_service -d pg_sql' -i example_pg_sql. This example will create a folder \".../{ROOT_FOLDER_NAME}/server/target_service/database/example_pg_sql\". This new folder will be used to store files needed for the database container to run." }
SERVICE_NAME_OPTION                = { "type": OptionsType.TYPE,   "value": str,          "required": True,  "help":  "Name of the created service." }
SERVICE_PATH_OPTION                = { "type": OptionsType.TYPE,   "value": str,          "required": False, "help": f"(Optional) Path of the service relative to root folder (\"{ROOT_FOLDER_NAME}\"). Example: 'python devola.py service -d pg_sql -i test -n new_service -sp \"server/alt_services\"'. This will result in a new docker compose database yaml file in \".../new_service/docker\" and the new folder \".../{ROOT_FOLDER_NAME}/server/alt_services/new_service/database/test\". This argument can also be used when creating a new service." }
VOLUMES_OPTION                     = { "type": OptionsType.ACTION, "value": "store_true", "required": False, "help":  "(Optional) Removes the volumes during cleaning." }