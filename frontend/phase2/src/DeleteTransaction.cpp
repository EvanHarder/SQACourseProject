// DeleteTransaction.cpp
//
// Method definitions for DeleteTransaction. See DeleteTransaction.h for what
// the class is for.

#include "DeleteTransaction.h"

#include "TransactionLine.h"

// Returns the command word "delete".
std::string DeleteTransaction::getName() const {
    return Config::COMMAND_DELETE;
}

// Asks for a username and deletes that account, or prints why it can't.
void DeleteTransaction::execute(Session& session, Console& console) {
    User* found = readExistingUser(session, console, "Enter username:");
    if (found == nullptr) {
        return;
    }
    if (found->getName() == session.getCurrentUser().getName()) {
        console.printError("cannot delete yourself.");
        return;
    }

    // Copy the user before removing them: removeUser rebuilds the user list,
    // so 'found' no longer points at a valid user afterwards
    const User deletedUser = *found;
    session.removeUser(deletedUser.getName());

    session.recordTransaction(TransactionLine::forAccount(
        Config::CODE_DELETE, deletedUser.getName(), deletedUser.getType(), deletedUser.getCredit()));
    console.print("User " + deletedUser.getName() + " deleted.");
}
