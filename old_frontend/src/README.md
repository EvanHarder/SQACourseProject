# Front End source (C++)

## Folders
| Folder | What goes in it |
|---|---|
| `main.cpp` | Program entry point: checks arguments, starts the FrontEnd |
| `io/` | Reading input and printing output: InputReader, ResultPrinter |
| `core/` | Session control: FrontEnd (main loop), Session (logged-in user and session state) |
| `data/` | Data files: UserAccounts, AvailableGames, GameCollection, TransactionLog, FormatUtils |
| `transactions/` | TransactionHandler: checks permissions and runs each transaction |

## Rules
- One class per `.h` / `.cpp` pair, named after the class (e.g. `UserAccounts.h`, `UserAccounts.cpp`).
- Include other headers by folder path from `src/`, e.g. `#include "data/UserAccounts.h"`.
- Every `.h` starts with a comment: class name and intention (one sentence).
- Every method has a one-line comment saying what it does.
- Keep parameter lists and calls on one line.
- Don't change another member's public method signatures without telling the group.

## Build (from the repository root)
```
make            # Linux: g++, produces frontend
mingw32-make    # Windows: clang++ -static, produces frontend.exe
```
`make clean` (or `mingw32-make clean`) deletes the program and object files.

## Run one test
Linux:
```
./frontend TestCases/data/current_user_accounts.txt TestCases/data/available_games.txt TestCases/data/game_collection.txt daily_transaction.txt < TestCases/login/success/login01/input.txt
```
Windows (PowerShell):
```
Get-Content TestCases\login\success\login01\input.txt | .\frontend.exe TestCases\data\current_user_accounts.txt TestCases\data\available_games.txt TestCases\data\game_collection.txt daily_transaction.txt
```

## Run the tests
```
make test                           # all tests (mingw32-make test on Windows)
./scripts/run_tests.sh ./frontend login   # one feature only (Linux)
```