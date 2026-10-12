# core/
Runs the session.

- `FrontEnd.h`: agreed interface used by main.cpp (constructor + run()). Keep these two signatures.
- `FrontEnd.cpp`: main loop, reads each transaction code and calls the right TransactionHandler method (to be assigned)
- `Session.h/.cpp`: logged-in user and what changed this session (to be assigned)
