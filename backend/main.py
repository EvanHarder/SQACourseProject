# main.py
#
# Entry point of the Back End. Run once a day, after all Front End sessions:
#
#   python backend/main.py <accounts> <games> <collection> <merged_transactions> <new_accounts> <new_games> <new_collection>
#
# It reads the current user accounts, available games and game collection
# files, applies every transaction in the merged daily transaction file, and
# writes the new versions of the three files for the next day's Front End.
#
# Classes:
#   BackEnd  holds the seven file paths and runs the steps in order

import sys

from data_files import AccountsFile, GamesFile, CollectionFile
from error_log import ErrorLog
from processor import TransactionProcessor
from transaction_file import TransactionFileReader

USAGE = "usage: python main.py <accounts> <games> <collection> <merged_transactions> <new_accounts> <new_games> <new_collection>"


class BackEnd:

    # Stores the seven file paths, in the order they are given on the command line.
    def __init__(self, paths):
        (self.accounts_path, self.games_path, self.collection_path, self.transactions_path,
         self.new_accounts_path, self.new_games_path, self.new_collection_path) = paths

    # Reads the old files, applies the day's transactions and writes the new files.
    def run(self):
        # 1. Read everything first, so a bad input file stops the Back End
        #    before any new file is written
        users = AccountsFile.read(self.accounts_path)
        games = GamesFile.read(self.games_path)
        collection = CollectionFile.read(self.collection_path)
        transactions = TransactionFileReader.read(self.transactions_path)

        # 2. Apply the transactions in the order the Front End recorded them
        processor = TransactionProcessor(users, games, collection, ErrorLog)
        processor.process_all(transactions)

        # 3. Write the new files
        #    (the processor keeps users and games as dictionaries, so write their values)
        AccountsFile.write(self.new_accounts_path, processor.users.values())
        GamesFile.write(self.new_games_path, processor.games.values())
        CollectionFile.write(self.new_collection_path, processor.collection)


# Checks the command line and runs the Back End.
def main():
    if len(sys.argv) != 8:
        print(USAGE)
        sys.exit(1)
    BackEnd(sys.argv[1:]).run()


if __name__ == "__main__":
    main()