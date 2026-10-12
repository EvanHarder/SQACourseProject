# Front End source (C++)

## Folders
| Folder | What goes in it | Owner |
|---|---|---|
| `main.cpp` | Program entry point: checks arguments, starts the FrontEnd | Member B |
| `io/` | Reading input and printing output: InputReader, ResultPrinter | Member B (InputReader), ResultPrinter: to be assigned |
| `core/` | Session control: FrontEnd (main loop), Session (logged-in user and session state) | to be assigned |
| `data/` | Data files: UserAccounts, AvailableGames, GameCollection, TransactionLog, FormatUtils | Member A |
| `transactions/` | TransactionHandler: one method per transaction | Member C |

## Rules
- One class per `.h` / `.cpp` pair, named after the class (e.g. `UserAccounts.h`, `UserAccounts.cpp`).
- Include other headers by folder path from `src/`, e.g. `#include "data/UserAccounts.h"`.
- Every `.h` starts with a comment: class name, intention (one sentence), owner.
- Every method has a one-line comment saying what it does.
- Don't change another member's public method signatures without telling the group.

## Build (from the repository root)
```
g++ -std=c++17 -Wall -I src -o frontend src/main.cpp src/*/*.cpp
```
On Windows this produces `frontend.exe`.

## Run one test
```
./frontend TestCases/data/current_user_accounts.txt TestCases/data/available_games.txt TestCases/data/game_collection.txt daily_transaction.txt < TestCases/login/success/login01/input.txt
```

## Run all tests for one feature
```
./scripts/run_tests.sh ./frontend login
```
