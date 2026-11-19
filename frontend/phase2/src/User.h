// User.h
//
// User
// One account from the current user accounts file (or one made by create):
// a username, a two-letter user type and an available credit balance.
//
// The username and type never change once a user exists (no transaction
// renames a user or changes their type), so they have no setters. Credit only
// changes through addCredit and removeCredit. Limit checks (enough money, the
// $999,999.99 cap) are done by the transactions before calling these, because
// each transaction prints its own error message.

#pragma once

#include <string>

#include "Constants.h"

class User {
public:
    // Creates a user with the given username, type code (AA/FS/BS/SS/AM) and credit in cents.
    User(const std::string& name, const std::string& type, Cents credit);

    // Returns the username.
    std::string getName() const;

    // Returns the two-letter user type code.
    std::string getType() const;

    // Returns the available credit in cents.
    Cents getCredit() const;

    // Returns true if this user is an admin (AA). Only used where AA itself matters,
    // such as an account manager not being allowed to create or delete admins.
    bool isAdmin() const;

    // Returns true if this user is an account manager (AM).
    bool isAccountManager() const;

    // Returns true if this user may run privileged transactions: admins (AA)
    // and account managers (AM).
    bool hasAdminPrivileges() const;

    // Returns true if this user may buy games (every type except sell-standard and account manager).
    bool canBuy() const;

    // Returns true if this user may sell games (every type except buy-standard and account manager).
    bool canSell() const;

    // Adds the given amount (in cents) to this user's credit.
    void addCredit(Cents amount);

    // Subtracts the given amount (in cents) from this user's credit.
    void removeCredit(Cents amount);

    // Returns true if the text is one of the five user type codes (AA, FS, BS, SS, AM).
    static bool isValidType(const std::string& type);

private:
    std::string name;  // the username, at most Config::MAX_USERNAME_LENGTH characters
    std::string type;  // two-letter user type code
    Cents credit;      // available credit, in cents
};
