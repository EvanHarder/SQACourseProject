// ListAccountsTransaction.cpp
//
// Method definitions for ListAccountsTransaction. See ListAccountsTransaction.h
// for what the class is for.

#include "ListAccountsTransaction.h"

#include "Formatter.h"

// Returns the command word for listing accounts.
std::string ListAccountsTransaction::getName() const {
    return Config::COMMAND_LIST_ACCOUNTS;
}

// Prints every active account as "username - type - credit".
// There is always at least one account: the admin running the command.
void ListAccountsTransaction::execute(Session& session, Console& console) {
    console.print("Active accounts:");
    for (const User& user : session.getUsers()) {
        console.print(user.getName() + " - " + user.getType() + " - " + Formatter::formatMoney(user.getCredit()));
    }
}
