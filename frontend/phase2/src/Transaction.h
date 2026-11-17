// Transaction.h
//
// Transaction
// The base class every Front End command inherits from (login, sell, buy, ...).
// FrontEnd keeps one object per command and, for each command typed:
//   1. refuses it if requiresLogin() and nobody is logged in,
//   2. refuses it if isAllowedFor(current user) is false,
//   3. otherwise calls execute(), which asks the command's questions,
//      validates the answers and records the daily transaction file line.
// Steps 1 and 2 happen once in FrontEnd, so no subclass repeats those checks.
//
// Questions that several commands ask the same way (an existing username, an
// amount of money) are shared helpers here, so their prompts and error
// messages are written once.

#pragma once

#include <string>

#include "Console.h"
#include "Constants.h"
#include "Session.h"
#include "User.h"

class Transaction {
public:
    virtual ~Transaction() = default;

    // Returns the command word the user types to run this transaction, e.g. "sell".
    virtual std::string getName() const = 0;

    // Returns true if this transaction can only run while someone is logged in.
    // True for every command except login.
    virtual bool requiresLogin() const;

    // Returns true if the given logged-in user's type may run this transaction.
    // Anyone may by default; subclasses with restrictions override this.
    virtual bool isAllowedFor(const User& user) const;

    // Asks this transaction's questions, checks the answers, updates the session
    // and records its daily transaction file line. On bad input it prints one
    // error and returns without changing anything.
    virtual void execute(Session& session, Console& console) = 0;

protected:
    // Asks for a username with the given prompt and returns that user.
    // Prints "user does not exist." and returns nullptr if there is no such account.
    // The pointer is only valid until users are next added or removed.
    static User* readExistingUser(Session& session, Console& console, const std::string& promptText);

    // Asks for an amount of money with the given prompt and stores it in amount (in cents).
    // Prints errorMessage and returns false if the answer isn't a valid amount.
    // Range checks (zero, maximums) are left to each transaction.
    static bool readMoney(Console& console, const std::string& promptText,
                          const std::string& errorMessage, Cents& amount);
};
