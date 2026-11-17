// LogoutTransaction.cpp
//
// Method definitions for LogoutTransaction. See LogoutTransaction.h for what
// the class is for.

#include "LogoutTransaction.h"

#include "TransactionLine.h"

// Returns the command word "logout".
std::string LogoutTransaction::getName() const {
    return Config::COMMAND_LOGOUT;
}

// Writes the session's transactions (ending with the 00 line) to the daily
// transaction file and logs out.
void LogoutTransaction::execute(Session& session, Console& console) {
    session.recordTransaction(TransactionLine::forEndOfSession());
    session.logout();
    console.print("Logout successful.");
}
