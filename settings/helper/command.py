from argparse import Namespace
from abc import ABC, abstractmethod
from . import errors

class Command(ABC):
    def __init__():
        pass

    @abstractmethod
    def arguments(self) -> dict[str, bool]:
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
    def manual(self) -> str:
        """
        Gives the detailed description on how to use the command.
        """
        pass

    def validate_arguments(self, args: Namespace):
        for argument, required in self.arguments().items():
            if required and not getattr(args, argument, None):
                raise errors.InvalidInputError(f"Missing argument \"--{argument}\" !\n\
                                                 Try \"{self.command()} -man\" to learn how to use this command properly.")