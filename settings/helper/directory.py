class DirectoryChanger():
    def __init__(self):
        pass

    def change_to_build_dir_command(self) -> str:
        return "cd server; mkdir -p build; cd build;"
    
    def reset_directory_command(self) -> str:
        return "cd ../..;"