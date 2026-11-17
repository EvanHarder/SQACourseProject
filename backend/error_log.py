# error_log.py
#
# ErrorLog
# Prints the Back End's two kinds of errors, in the formats the spec asks for:
#   - constraint errors: a transaction breaks a rule (for example a negative
#     balance or a username that already exists). The transaction is skipped,
#     the message is printed and the Back End keeps going.
#   - fatal errors: an input file is missing or badly formatted. The message
#     names the file, and the Back End stops without writing any new files.
#
# Both methods are static, so any class can call ErrorLog.fatal(...) without
# needing an ErrorLog object passed to it.

import sys


class ErrorLog:

    # Prints a constraint error as "ERROR: <message>" and lets the Back End continue.
    @staticmethod
    def constraint(message):
        print("ERROR: " + message)

    # Prints a fatal error as "ERROR: <file_name>: <message>" and stops the Back End.
    @staticmethod
    def fatal(file_name, message):
        print("ERROR: " + file_name + ": " + message)
        sys.exit(1)
