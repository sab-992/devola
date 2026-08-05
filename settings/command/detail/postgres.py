import os
import subprocess
from pathlib import Path

from settings.command.detail.file_system import FileSystem
from settings.command.detail.log import log, Color
from settings.config import SERVICES_PATH


class Postgres():
    def __init__(self):
        self.fs = FileSystem()
        self.db_name = "devola"
        self.host = "localhost"
        self.port = "5432"
        self.admin_user = "postgres"
        self.app_user = "devola_app"

    def setup(self):
        current_dir = str(Path(__file__).resolve().parent)
        setup_file = os.path.join(current_dir, "setup.sql")

        self.create_pg_user()
        self.run_file(setup_file, self.admin_user)

    def run(self) -> None:
        services_dir = os.path.join(self.fs.find_root_folder(), SERVICES_PATH)

        for service in self.fs.get_services():
            service_database_dir = os.path.join(services_dir, service, "database")

            if os.path.exists(service_database_dir):
                self.run_sql_folder(service_database_dir, "schema.sql")
                self.run_sql_folder(service_database_dir, "tables.sql")
                self.run_sql_folder(service_database_dir, "constraints.sql")
                self.run_sql_folder(service_database_dir, "indexes.sql")
                self.run_sql_folder(service_database_dir, "procedures.sql")
            else:
                log(f"No folder: {service_database_dir}", False, Color.RED)

    def run_file(self, filepath, user):
        cmd = self.base_psql_command(user) + ["-f", str(filepath), "-v", "ON_ERROR_STOP=1"]
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            raise RuntimeError(f"Failed running {filepath} as '{user}': {result.stderr}")

    def run_sql_folder(self, folder_path: str, targeted_file: str):
        for sql_file in sorted(Path(folder_path).rglob(f"*{targeted_file}")):
            self.run_file(sql_file, self.admin_user if sql_file.name.endswith("schema.sql") else self.app_user)

    def create_pg_user(self):
        from dotenv import load_dotenv

        env_path = os.path.join(self.fs.find_root_folder(), "settings", ".env")
        load_dotenv(env_path)
        password = os.getenv("POSTGRES_APP_USER_PASSWORD")

        if (not password):
            raise RuntimeError(f"PGSQL: No user password given")

        escaped_password = password.replace("'", "''")
        sql = f"CREATE USER {self.app_user} WITH PASSWORD '{escaped_password}';"

        subprocess.run(self.base_psql_command(self.admin_user) + ["-c", sql], check=True)

    def base_psql_command(self, user):
        return ["psql", "-h", self.host, "-p", self.port, "-d", self.db_name, "-U", user]