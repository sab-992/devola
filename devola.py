import os
import subprocess
import argparse
from settings.helper.command import Command
from settings.helper.log import log, Color

from settings import compile, debug, deploy, service, shutdown, test

COMMANDS: dict[str, Command] = { "cmake"       : compile.CMake(), 
                                 "make"        : compile.Make(),
                                 "makeall"     : compile.MakeAll(),
                                 "debug"       : debug.Debug(),
                                 "deploy"      : deploy.Deploy(),
                                 "add_service" : service.AddService(),
                                 "shutdown"    : shutdown.Shutdown(),
                                 "test"        : test.Test() }

def main():
    parser = argparse.ArgumentParser(description="Server deployment and stop script.")
    # Main command
    parser.add_argument("command", help="Command < add_service | deploy | make | cmake | makeall | debug | shutdown | test >")

    parser.add_argument("-c", "--clean", action="store_true", help="(For 'shutdown' only) Removes EVERYTHING about Docker (for development and test modes ONLY), \
                                                                    it also removes the database for every mode except 'Production'.")
    parser.add_argument("-l", "--logs", type=str, help="(For 'deploy' only) Display logs for the chosen docker")
    parser.add_argument("-n", "--name", type=str, help="(For 'add_service' only) Name of the created service.")
    parser.add_argument("-p", "--prod", action="store_true", help="(For 'deploy' only) Deploy in production mode.")
    parser.add_argument("-r", "--regex", type=str, help="(For 'tests' only) To launch specific tests")
    parser.add_argument("-man", "--manual", action="store_true", help="Describes a specific command.")
    parser.add_argument("-d", "--debug", action="store_true", help="(For 'make' and 'makeall' only) Starts the application in debug mode.")
    parser.add_argument("-sp", "--service_path", type=str, help="(For 'add_service' only) Path of the created service.")

    args = parser.parse_args()

    try:
        env = os.environ.copy()
        if args.command:
            subprocess.run(COMMANDS[args.command].command_explicit(args), shell=True, env=env)
    except Exception as e:
        log(f"Something went wrong while running {args.command}: {e}", False, Color.RED)

if __name__ == "__main__":
    main()