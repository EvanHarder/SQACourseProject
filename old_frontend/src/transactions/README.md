# transactions/
The transactions themselves.

- `TransactionHandler.h/.cpp`: `handleTransaction` checks login and permissions, then calls one method per transaction:
  handleLogin, handleLogout, handleCreate, handleDelete, handleSell, handleBuy, handleRefund,
  handleAddCredit, handleList, handleListAccounts
- Phase 2: every transaction checks its rules; writing the daily transaction file and some data changes
  (marked PLACEHOLDER in the code) will be finished in Phase 3