// CreateTransaction.cpp
//
// Method definitions for CreateTransaction. See CreateTransaction.h for what
// the class is for.

#include "CreateTransaction.h"

#include "TransactionLine.h"

// Returns the command word "create".
std::string CreateTransaction::getName() const {
    return Config::COMMAND_CREATE;
}

// Asks for a username and user type and creates the account, or prints why it can't.
void CreateTransaction::execute(Session& session, Console& console) {
    std::string username = console.prompt("Enter username:");
    std::string problem = findUsernameProblem(session, username);
    if (!problem.empty()) {
        console.printError(problem);
        return;
    }

    std::string userType = console.prompt("Enter user type:");
    if (!User::isValidType(userType)) {
        console.printError("invalid user type.");
        return;
    }

    // New accounts start with no credit (client clarification)
    const Cents startingCredit = 0;
    session.addUser(User(username, userType, startingCredit));
    session.recordTransaction(
        TransactionLine::forAccount(Config::CODE_CREATE, username, userType, startingCredit));
    console.print("User " + username + " created.");
}

// Returns the error message for an unusable new username, or "" if it is fine.
// Blank names, names with spaces and the END marker can't be stored in the files.
std::string CreateTransaction::findUsernameProblem(Session& session, const std::string& username) const {
    if (username.empty() || username.find(' ') != std::string::npos || username == Config::END_MARKER) {
        return "invalid username.";
    }
    if (username.size() > Config::MAX_USERNAME_LENGTH) {
        return "username too long.";
    }
    // Includes users created earlier this session; a name freed by delete can be reused
    if (session.findUser(username) != nullptr) {
        return "username already exists.";
    }
    return "";
}
