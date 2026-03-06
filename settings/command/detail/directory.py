import os

from .file_system import FileSystem

class Directory():
    def __init__(self):
        pass

    def build_directory(self) -> str:
        build_directory_path = f"{FileSystem().find_root_folder()}/build"
        os.makedirs(build_directory_path, exist_ok=True)
        return build_directory_path

    def root_directory(self) -> str:
        return FileSystem().find_root_folder()