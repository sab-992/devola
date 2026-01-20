import os
import subprocess

from abc import ABC, abstractmethod
from argparse import Namespace

from settings.command.detail.log import log, Color
from .errors import InvalidInputError


class Command(ABC):
    def __init__(self):
        self.working_dir = ""

    @abstractmethod
    def arguments(self) -> dict[str, dict]:
        """
        Gives the accepted arguments (key) and their requirement status (value).
        """
        pass

    @abstractmethod
    def command(self) -> str:
        """
        Gives the name of the command.
        """

    @abstractmethod
    def command_explicit(self, args: Namespace) -> str:
        """
        Gives the command to execute.
        """
        pass

    @abstractmethod
    def details(self) -> str:
        """
        Explain what the command do.
        """
        pass

    def run(self, args: Namespace) -> None:
        """
        Runs the current command.
        """
        if args.manual:
            return print(f"{self.manual()}")

        try:
            self.validate_arguments(args)
            command = self.command_explicit(args)

            if not command:
                return

            self.setup(args)
            subprocess.run(command, cwd=self.working_dir if self.working_dir and len(self.working_dir) != 0 else None, shell=True, env=os.environ.copy())
            self.teardown(args)
        except Exception as e:
            log(f"Something went wrong while running the command \"{self.command}\": {e}", True, Color.RED)

    def manual(self) -> str:
        """
        Gives the detailed description on how to use the command.
        """
        options = "Options:\n"
        for argument, arguments_info in self.arguments().items():
            options += f"\t-{argument[0]}, {argument} - {arguments_info["help"]}\n"
        return f"{self.details()}\n\n{options[:-1]}"
    
    def set_working_directory(self, working_dir: Namespace) -> str:
        self.working_dir = working_dir
    
    def get_working_directory(self) -> str:
        return self.working_dir

    @abstractmethod
    def setup(self, args: Namespace) -> str:
        """
        Sets the environment for the command.
        """
        pass

    @abstractmethod
    def teardown(self, args: Namespace) -> str:
        """
        Restores the environment after the command.
        """
        pass

    def validate_arguments(self, args: Namespace):
        """
        Verifies that all required arguments have been given.
        """
        for argument, argument_infos in self.arguments().items():
            if argument_infos.get("required", True) and not getattr(args, argument, None):
                raise InvalidInputError(f"Missing argument \"--{argument}\" !\n\
                                          Try \"{self.command()} -man\" to learn how to use this command properly.")