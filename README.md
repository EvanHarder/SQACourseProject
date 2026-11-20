# Digital Games Distribution System

CSCI 3060U Software Quality Assurance, Fall 2026, Ontario Tech University
Team: Evan Harder, Muhammad Niazi, Maheen Zahir

A digital game store in two programs:
- **Front End** (C++17): a console terminal. Each session reads the current data files, runs one user's
  transactions and writes a daily transaction file.
- **Back End** (Python 3): runs once a day. It applies every session's transactions to the data files and
  writes the new data files for the next day's Front End.

Since Phase 4 we use another team's Front End (Front End A), as the project requires. Our own Phase 1 to 3
Front End is kept in `old_frontend/` (Git tag `phase3-final`).

## Folder layout
| Folder | Contents |
|---|---|
| `frontend/phase2/` | Front End A source (`src/`), `Makefile` and sample data files (`data/`) |
| `frontend/phase3/` | Front End A tests: `run_tests.sh`, `check_outputs.sh`, `tests/inputs`, `tests/expected`, `tests/data` |
| `backend/` | Python Back End: `main.py`, `models.py`, `data_files.py`, `transaction_file.py`, `processor.py`, `error_log.py` |
| `backend/sample/` | Sample input files for the Back End, with the expected new files in `backend/sample/expected/` |
| `old_frontend/` | Our Phase 1 to 3 Front End and its 155 tests (see `old_frontend/` for its own instructions) |
| `AI_Acknowledgement/` | AI prompt and response history for each phase |

## Requirements
- A C++17 compiler (`g++` or `clang++`) and `make`
- Python 3.8 or newer (standard library only)
- `bash` and `diff` for the test scripts

## Front End

### Build
```
cd frontend/phase2
make            # produces ./frontend
make clean      # deletes build/ and the program
```

### Run
```
./frontend <current_user_accounts> <available_games> <game_collection> <daily_transaction_file>
```
Example with the sample data:
```
./frontend data/users.txt data/games.txt data/collection.txt daily_transaction.atf
```
Type a command, then answer each prompt on its own line. `logout` writes the daily transaction file.
Exit with Ctrl+D (Linux/Mac) or Ctrl+Z then Enter (Windows).

| Command | Prompts | Who can use it |
|---|---|---|
| login | username | anyone, when no one is logged in |
| logout | none | logged-in user |
| create | username, user type (AA, FS, BS, SS, AM) | admin, account manager (an AM cannot create AA) |
| delete | username | admin, account manager (an AM cannot delete AA) |
| sell | game name, price | everyone except BS and AM |
| buy | game name, seller | everyone except SS and AM |
| refund | buyer, seller, amount | admin, account manager |
| addcredit | amount, then username for admins and account managers | logged-in user |
| list | none | logged-in user |
| listaccounts | none | admin, account manager |
| search | query | logged-in user (non-admins don't see admins who aren't selling) |

User types: AA admin, FS full-standard, BS buy-standard, SS sell-standard, AM account manager
(admin privileges, but cannot buy, sell, or create or delete admins).

### Test
```
cd frontend/phase3
./run_tests.sh          # builds the Front End, runs all 161 tests, then checks them
./run_tests.sh am       # only tests whose names start with "am"
```
Each run saves its actual outputs in `outputs/<date>_<time>/`, with `report.txt` (pass/fail per test) and
`failures.txt` (the differences). Expected result: `Total: 161  Passed: 161  Failed: 0`.

## Back End

### Run
```
python3 backend/main.py <accounts> <games> <collection> <merged_transactions> <new_accounts> <new_games> <new_collection>
```
Example with the sample files:
```
python3 backend/main.py backend/sample/accounts.txt backend/sample/games.txt backend/sample/collection.txt backend/sample/merged_transactions.txt new_accounts.txt new_games.txt new_collection.txt
```
Expected output (two transactions in the sample break a rule on purpose):
```
ERROR: insufficient credit.
ERROR: username already exists.
```
Check the new files against the expected ones (no output means they match):
```
diff new_accounts.txt backend/sample/expected/new_accounts.txt
diff new_games.txt backend/sample/expected/new_games.txt
diff new_collection.txt backend/sample/expected/new_collection.txt
```
A rule violation (such as a negative balance or a duplicate username) prints `ERROR: <message>`, skips that
transaction and continues. A missing or badly formatted input file prints `ERROR: <file>: line <n>: <problem>`
and stops without writing any new files.

## A full day: Front End sessions, then the Back End
From the repository root, after building the Front End:
```
F=frontend/phase2
mkdir -p day

# Two Front End sessions, each writing its own daily transaction file
printf "login\nadmin\ncreate\nnewuser\nFS\naddcredit\n50\nfulluser\nlogout\n" | $F/frontend $F/data/users.txt $F/data/games.txt $F/data/collection.txt day/session1.atf
printf "login\nbuyuser\nbuy\nStardew Valley\nselluser\nlogout\n" | $F/frontend $F/data/users.txt $F/data/games.txt $F/data/collection.txt day/session2.atf

# Merge the sessions and run the Back End
cat day/session1.atf day/session2.atf > day/merged.atf
python3 backend/main.py $F/data/users.txt $F/data/games.txt $F/data/collection.txt day/merged.atf day/users.txt day/games.txt day/collection.txt

# The next day's Front End uses the Back End's new files: newuser can now log in
printf "login\nnewuser\nlogout\n" | $F/frontend day/users.txt day/games.txt day/collection.txt day/session3.atf
```
After the Back End runs, `day/users.txt` has newuser, fulluser has 50.00 more credit, Stardew Valley's price
has moved from buyuser to selluser, and `day/collection.txt` shows buyuser owning Stardew Valley.

## Windows notes
- Use the MSYS2 UCRT64 terminal for `make` and the test scripts. Install the tools once with
  `pacman -S --needed make diffutils mingw-w64-ucrt-x86_64-clang`.
- If `g++` crashes, build with clang instead: `export MAKEFLAGS='CXX=clang++ CXXFLAGS=-std=c++17\ -static'`
  before `make` or `./run_tests.sh`.
- Windows programs write `\r\n` line endings, so the tests can report false failures. Strip them from a run and
  re-check it: `sed -i 's/\r$//' outputs/<run>/*.out outputs/<run>/*.atf` then `bash check_outputs.sh outputs/<run>`.
- Run the Back End with `python` instead of `python3`.