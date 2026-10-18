# data/
Loading and saving the data files and writing the daily transaction file.

All three input files are fixed width and end with an `END` line; names may contain spaces.

- `FormatUtils.h/.cpp`: pad and trim text fields, convert money to and from text (e.g. 005000.00)
- `UserAccounts.h/.cpp`: load and save current_user_accounts.txt, find users and update their credit
- `AvailableGames.h/.cpp`: load and save available_games.txt, find games for sale
- `GameCollection.h/.cpp`: load and save game_collection.txt, check whether a user owns a game
- `TransactionLog.h/.cpp`: store successful transactions, write the daily transaction file at logout (first version)