import os
import subprocess
import argparse
from settings.helper.command import Command
from settings.helper.log import log, Color
from settings import compile, debug, deploy, service, shutdown, test
from settings.helper.options import OptionsType

COMMANDS: dict[str, Command] = { "cmake"       : compile.CMake(), 
                                 "make"        : compile.Make(),
                                 "makeall"     : compile.MakeAll(),
                                 "debug"       : debug.Debug(),
                                 "deploy"      : deploy.Deploy(),
                                 "add_service" : service.AddService(),
                                 "shutdown"    : shutdown.Shutdown(),
                                 "test"        : test.Test() }

def main():
    parser = argparse.ArgumentParser(description="Application deployment and stop script.")
    added_arguments = set()
    program_help = "<"
    for command_name, command in COMMANDS.items():
        for argument, argument_infos in command.arguments().items():
            if argument in added_arguments:
                continue

            if argument_infos["type"] == OptionsType.ACTION:
                parser.add_argument(f"-{argument[0]}", f"--{argument}", action=argument_infos["value"], help=argument_infos["help"])
            else:
                parser.add_argument(f"-{argument[0]}", f"--{argument}", type=argument_infos["value"], help=argument_infos["help"])

            added_arguments.add(argument)
        program_help += f" {command_name} |"
        
    program_help = program_help[:-1] + ">"
    parser.add_argument("command", help=program_help)

    args = parser.parse_args()

    try:
        env = os.environ.copy()
        if args.command:
            subprocess.run(COMMANDS[args.command].command_explicit(args), shell=True, env=env)
    except Exception as e:
        log(f"Something went wrong while running {args.command}: {e}", False, Color.RED)

if __name__ == "__main__":
    main()