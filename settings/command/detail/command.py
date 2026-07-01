import os
import subprocess

from abc import ABC, abstractmethod
from argparse import Namespace

from settings.command.detail.log import log, Color
from .errors import InvalidInputError


class Command(ABC):
    def __init__(self):
        self.initial_working_dir = os.getcwd()

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
    def command_explicit(self, args: Namespace) -> list[list[str]]:
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
            commands = self.command_explicit(args)

            if len(commands) <= 0:
                return

            self.setup(args)
            for command in commands:
                subprocess.run(command, env=os.environ.copy(), check=True)
            self.teardown(args)
        except Exception as e:
            log(f"Something went wrong while running the command \"{self.command}\": {e}", True, Color.RED)

    def manual(self) -> str:
        """
        Gives the detailed description on how to use the command.
        """
        options = "Options:\n"
        for argument, arguments_info in self.arguments().items():
            words_in_arg = argument.split("_")
            abbrev = ""
            for word in words_in_arg:
                abbrev += word[0]

            options += f"\t-{abbrev}, {argument} - {arguments_info["help"]}\n"
        return f"{self.details()}\n\n{options[:-1]}"

    def reset_working_directory(self) -> None:
        os.chdir(self.initial_working_dir)

    def set_working_directory(self, working_dir: Namespace) -> None:
        os.chdir(working_dir)

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