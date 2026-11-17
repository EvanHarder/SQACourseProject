// Transaction.cpp
//
// Default behaviour and shared helpers for Transaction. See Transaction.h for
// how FrontEnd uses these.

#include "Transaction.h"

#include "Formatter.h"

// Every command except login needs someone logged in.
bool Transaction::requiresLogin() const {
    return true;
}

// Any logged-in user may run the transaction unless a subclass says otherwise.
bool Transaction::isAllowedFor(const User&) const {
    return true;
}

// Asks for a username and returns that user, or prints an error and returns nullptr.
User* Transaction::readExistingUser(Session& session, Console& console, const std::string& promptText) {
    std::string username = console.prompt(promptText);
    User* user = session.findUser(username);
    if (user == nullptr) {
        console.printError("user does not exist.");
    }
    return user;
}

// Asks for an amount of money and stores it in cents, or prints errorMessage and returns false.
bool Transaction::readMoney(Console& console, const std::string& promptText,
                            const std::string& errorMessage, Cents& amount) {
    std::string answer = console.prompt(promptText);
    if (!Formatter::parseMoney(answer, amount)) {
        console.printError(errorMessage);
        return false;
    }
    return true;
}
