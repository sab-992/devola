import os

from .file_system import FileSystem

class Directory():
    def __init__(self):
        pass

    def build_directory(self) -> str:
        return f"{FileSystem().find_root_folder()}/build"

    def root_directory(self) -> str:
        return FileSystem().find_root_folder()