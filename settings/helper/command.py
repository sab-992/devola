import os
import subprocess

from argparse import Namespace
from abc import ABC, abstractmethod
from . import errors
from settings.helper.log import log, Color


class Command(ABC):
    def __init__():
        pass

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
        if args.manual:
            return print(f"{self.manual()}")

        log("test")

        try:
            self.validate_arguments(args)
            command = self.command_explicit(args)
            if command:
                subprocess.run(command, shell=True, env=os.environ.copy())
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

    def validate_arguments(self, args: Namespace):
        """
        Verifies that all required arguments have been given.
        """
        for argument, argument_infos in self.arguments().items():
            if argument_infos.get("required", True) and not getattr(args, argument, None):
                raise errors.InvalidInputError(f"Missing argument \"--{argument}\" !\n\
                                                 Try \"{self.command()} -man\" to learn how to use this command properly.")