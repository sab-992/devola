import os

from pathlib import Path
from .file_system import FileSystem

class Directory():
    def __init__(self):
        pass

    def build_directory(self) -> str:
        build_directory_path = f"{FileSystem().find_root_folder()}/build"
        os.makedirs(build_directory_path, exist_ok=True)
        return self.uniformizePath(build_directory_path)

    def root_directory(self) -> str:
        return str(FileSystem().find_root_folder())

    def uniformizePath(self, path: str):
        return str(Path(path))