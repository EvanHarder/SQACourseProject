# core/
Runs the session.

- `FrontEnd.h/.cpp`: creates the data objects and the TransactionHandler, then reads each transaction code and passes it to `handleTransaction` until the input ends. main.cpp uses the constructor and `run()`; keep these two signatures.
- `Session.h/.cpp`: the logged-in user and their type (first version); later also what changed this session (deleted users, new games, credit added)