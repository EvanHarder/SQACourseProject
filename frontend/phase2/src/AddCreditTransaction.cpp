// AddCreditTransaction.cpp
//
// Method definitions for AddCreditTransaction. See AddCreditTransaction.h for
// what the class is for and the order of its checks.

#include "AddCreditTransaction.h"

#include "Formatter.h"
#include "TransactionLine.h"

// Returns the command word "addcredit".
std::string AddCreditTransaction::getName() const {
    return Config::COMMAND_ADD_CREDIT;
}

// Asks for an amount (and, for an admin, a username) and adds the credit,
// or prints why it can't.
void AddCreditTransaction::execute(Session& session, Console& console) {
    Cents amount = 0;
    if (!readMoney(console, "Enter amount:", "invalid amount.", amount)) {
        return;
    }
    if (amount == 0) {
        console.printError("invalid amount.");
        return;
    }

    // An admin chooses the account; anyone else adds to their own
    User* account = &session.getCurrentUser();
    if (account->isAdmin()) {
        account = readExistingUser(session, console, "Enter username:");
        if (account == nullptr) {
            return;
        }
    }

    // The $1000 limit is per account, per session, and applies to admins too
    Cents alreadyAdded = session.getCreditAddedThisSession(account->getName());
    if (alreadyAdded + amount > Config::MAX_CREDIT_PER_SESSION) {
        console.printError("session credit limit exceeded.");
        return;
    }
    if (account->getCredit() + amount > Config::MAX_CREDIT) {
        console.printError("credit limit exceeded.");
        return;
    }

    account->addCredit(amount);
    session.recordCreditAdded(account->getName(), amount);
    session.recordTransaction(TransactionLine::forAccount(
        Config::CODE_ADD_CREDIT, account->getName(), account->getType(), amount));
    console.print("Added " + Formatter::formatMoney(amount) + " credit to " + account->getName() + ".");
}
