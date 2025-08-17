from enum import Enum

class OptionsType(Enum):
    ACTION = 0
    TYPE = 1

CLEAN_OPTION        = { "type": OptionsType.ACTION, "value": "store_true", "required": False, "help": "Removes EVERYTHING about Docker (for development and test modes ONLY), it also removes the database for every mode except 'Production'." }
DEBUG_OPTION        = { "type": OptionsType.ACTION, "value": "store_true", "required": False, "help": "Starts the application in debug mode." }
LOGS_OPTION         = { "type": OptionsType.TYPE,   "value": str,          "required": False, "help": "Display logs for the chosen docker." }
MANUAL_OPTION       = { "type": OptionsType.ACTION, "value": "store_true", "required": False, "help": "Describes and explain a specific command." }
NAME_OPTION         = { "type": OptionsType.TYPE,   "value": str,          "required": True,  "help": "Name of the created service." }
PROD_OPTION         = { "type": OptionsType.ACTION, "value": "store_true", "required": False, "help": "Deploy in production mode." }
REGEX_OPTION        = { "type": OptionsType.TYPE,   "value": str,          "required": False, "help": "To launch specific tests" }
SERVICE_PATH_OPTION = { "type": OptionsType.TYPE,   "value": str,          "required": False, "help": "Path of the created service." }
