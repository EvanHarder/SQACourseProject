# data/
Loading the data files and writing the daily transaction file (Member A).

- `FormatUtils.h/.cpp`: pad text fields, convert money to and from text (e.g. 005000.00)
- `UserAccounts.h/.cpp`: load current_user_accounts.txt, find and update users
- `AvailableGames.h/.cpp`: load available_games.txt, find games
- `GameCollection.h/.cpp`: load game_collection.txt, check whether a user owns a game
- `TransactionLog.h/.cpp`: store successful transactions, write the daily transaction file at logout
