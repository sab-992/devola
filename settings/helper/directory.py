import os

class DirectoryChanger():
    def __init__(self):
        pass

    def change_to_build_dir_command(self) -> str:
        os.makedirs("../server/build", exist_ok=True)
        return "cd ./server/build"
    
    def reset_directory_command(self) -> str:
        return "cd ../.."