# Digital Games Distribution System: Front End

CSCI 3060U Software Quality Assurance, Fall 2026, Ontario Tech University
Team: Maheen Zahir, Evan Harder, Muhammad Niazi

The Front End is a console point-of-purchase terminal for a digital game store, written in C++17.
It reads one day's transactions from standard input, prints a result block for each one, and writes
every successful change to a daily transaction file for the Back End.

## Folder layout
| Folder / file | Contents |
|---|---|
| `src/` | Front End source code (see `src/README.md` for each class) |
| `TestCases/` | 155 requirements tests: `TestCases/<feature>/<success\|failure>/<test>/` with `input.txt`, `expected_output.txt`, `daily_transaction.txt`; shared data files in `TestCases/data/` |
| `scripts/` | Test scripts: `run_tests.sh` / `run_tests.bat`, `check_outputs.sh`, `compare_runs.sh` |
| `results/` | Saved test runs, one folder per run |
| `Makefile` | Builds, runs and tests the Front End |
| `AI_Acknowledgement/` | AI prompt and response history for each phase |

## Build
```
make            # Linux / Mac: g++, produces ./frontend
mingw32-make    # Windows (MSYS2): clang++ -static, produces frontend.exe
make clean      # delete the program and object files
```

## Run the Front End
```
make run                                                      # type transactions yourself
make run INPUT=TestCases/login/success/login01/input.txt      # read them from a file
```
On Windows use `mingw32-make run`. Without the Makefile:
```
./frontend <current_user_accounts> <available_games> <game_collection> <daily_transaction_file> < input.txt
```
Enter one `label: value` line at a time, starting with `transaction: login` and `username: admin01`.
When typing at the keyboard, a welcome screen lists every transaction. Finish with Ctrl+D (Linux/Mac)
or Ctrl+Z then Enter (Windows).

| Transaction | Fields it asks for | Who can use it |
|---|---|---|
| login | username | anyone |
| logout | none (writes the daily transaction file) | logged-in user |
| create | username, email, password, type (AA/FS/BS/SS) | admin |
| delete | username | admin |
| sell | game, price | all except buy-standard |
| buy | game, seller | all except sell-standard |
| refund | buyer, seller, amount | admin |
| addcredit | amount (admins: then username) | logged-in user |
| list | none | logged-in user |
| listaccounts | none | admin |
| search | query | logged-in user |

## Run the tests
```
make test                                          # run all 155 tests (mingw32-make test on Windows)
./scripts/run_tests.sh ./frontend buy              # one feature only
./scripts/check_outputs.sh                         # re-check the latest run against the expected outputs
./scripts/compare_runs.sh <older_run> <newer_run>  # what changed between two runs
```
Each run is saved in `results/run_<date>_<time>/` with `report.txt` (PASS/FAIL for every test) and, for each test,
`actual_output.txt`, `actual_daily_transaction.txt` and, for failing tests, `diff.txt`.