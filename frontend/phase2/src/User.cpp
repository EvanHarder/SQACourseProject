// User.cpp
//
// Method definitions for User. See User.h for what the class is for.

#include "User.h"

// Creates a user with the given username, type code and credit in cents.
User::User(const std::string& name, const std::string& type, Cents credit)
    : name(name), type(type), credit(credit) {}

// Returns the username.
std::string User::getName() const {
    return name;
}

// Returns the two-letter user type code.
std::string User::getType() const {
    return type;
}

// Returns the available credit in cents.
Cents User::getCredit() const {
    return credit;
}

// Returns true if this user is an admin (AA).
bool User::isAdmin() const {
    return type == Config::USER_TYPE_ADMIN;
}

// Returns true if this user is an account manager (AM).
bool User::isAccountManager() const {
    return type == Config::USER_TYPE_ACCOUNT_MANAGER;
}

// Returns true if this user may run privileged transactions: admins and account managers.
bool User::hasAdminPrivileges() const {
    return isAdmin() || isAccountManager();
}

// Returns true if this user may buy games: everyone except sell-standard (SS)
// and account managers (AM).
bool User::canBuy() const {
    return type != Config::USER_TYPE_SELL_STANDARD && !isAccountManager();
}

// Returns true if this user may sell games: everyone except buy-standard (BS)
// and account managers (AM).
bool User::canSell() const {
    return type != Config::USER_TYPE_BUY_STANDARD && !isAccountManager();
}

// Adds the given amount (in cents) to this user's credit.
// The caller has already checked that the result stays within Config::MAX_CREDIT.
void User::addCredit(Cents amount) {
    credit += amount;
}

// Subtracts the given amount (in cents) from this user's credit.
// The caller has already checked that the user has at least this much.
void User::removeCredit(Cents amount) {
    credit -= amount;
}

// Returns true if the text is exactly one of the five user type codes.
bool User::isValidType(const std::string& type) {
    return type == Config::USER_TYPE_ADMIN
        || type == Config::USER_TYPE_FULL_STANDARD
        || type == Config::USER_TYPE_BUY_STANDARD
        || type == Config::USER_TYPE_SELL_STANDARD
        || type == Config::USER_TYPE_ACCOUNT_MANAGER;
}
