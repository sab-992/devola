from enum import Enum


class Color(Enum):
    GREEN = '\033[0;32m'
    RED = '\033[0;31m'
    YELLOW = '\033[0;33m'
    BLUE = '\033[0;34m'
    PURPLE = '\033[0;35m'
    NC = '\033[0m'

def log(message: str, ressource_creation: bool=False, color=Color.GREEN):
    print(build_msg(message, ressource_creation, color))

def build_msg(message: str, ressource_creation: bool=False, color=Color.GREEN) -> str:
    if ressource_creation:
        return f"{color.value}{"[+]"if color != Color.RED else "[x]"}{Color.NC.value} {message}"
    else:
        return f"{color.value}{message}{Color.NC.value}"