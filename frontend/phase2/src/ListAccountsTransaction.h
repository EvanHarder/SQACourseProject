// ListAccountsTransaction.h
//
// ListAccountsTransaction
// The command that lists all active accounts (added by the client; admin
// only). Prints every current account with its user type and credit,
// including changes made this session. It writes nothing to the daily
// transaction file.
//
// "listaccounts" is a placeholder command word until the client confirms the
// official name (see Config::COMMAND_LIST_ACCOUNTS).

#pragma once

#include <string>

#include "PrivilegedTransaction.h"

class ListAccountsTransaction : public PrivilegedTransaction {
public:
    // Returns the command word for listing accounts.
    std::string getName() const override;

    // Prints every active account as "username - type - credit".
    void execute(Session& session, Console& console) override;
};
