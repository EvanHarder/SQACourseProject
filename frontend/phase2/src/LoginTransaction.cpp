// LoginTransaction.cpp
//
// Method definitions for LoginTransaction. See LoginTransaction.h for what
// the class is for.

#include "LoginTransaction.h"

// Returns the command word "login".
std::string LoginTransaction::getName() const {
    return Config::COMMAND_LOGIN;
}

// Returns false: login is the one command allowed before logging in.
bool LoginTransaction::requiresLogin() const {
    return false;
}

// Logs in the user whose username is entered, or prints why it can't.
void LoginTransaction::execute(Session& session, Console& console) {
    // Checked before asking anything: a second login must not end the current session
    if (session.isLoggedIn()) {
        console.printError("already logged in.");
        return;
    }

    std::string username = console.prompt("Enter username:");
    if (!session.login(username)) {
        console.printError("user does not exist.");
        return;
    }

    const User& user = session.getCurrentUser();
    console.print("Login successful. Welcome " + user.getName() + " (" + user.getType() + ").");
}
