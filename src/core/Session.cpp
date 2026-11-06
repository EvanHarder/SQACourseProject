/*
 * Session.cpp
 *
 * Implementation of the Session class: tracks the logged-in user and what
 * changed during the session.
 * See Session.h for the description of each method.
 */

#include "core/Session.h"

namespace {
    const char* ADMIN_TYPE = "AA";
}

// Starts with nobody logged in.
Session::Session() : loggedIn(false) {}

// Stores the user who just logged in.
void Session::start(const std::string& newUsername, const std::string& newUserType) {
    loggedIn = true;
    username = newUsername;
    userType = newUserType;
}

// Clears the logged-in user and everything recorded during the session.
void Session::end() {
    loggedIn = false;
    username.clear();
    userType.clear();
    deletedUsers.clear();
    newUsers.clear();
    newGames.clear();
    creditAddedCents.clear();
}

// Reports whether someone is logged in.
bool Session::isLoggedIn() const {
    return loggedIn;
}

// Returns the logged-in username.
const std::string& Session::getUsername() const {
    return username;
}

// Returns the logged-in user's type code.
const std::string& Session::getUserType() const {
    return userType;
}

// Admins have the type code AA.
bool Session::isAdmin() const {
    return loggedIn && userType == ADMIN_TYPE;
}

// Remembers the deleted user until logout.
void Session::markDeleted(const std::string& deletedUsername) {
    deletedUsers.insert(deletedUsername);
}

// Checks the users deleted this session.
bool Session::isDeleted(const std::string& checkUsername) const {
    return deletedUsers.count(checkUsername) > 0;
}

// Remembers the new user until logout.
void Session::markNewUser(const std::string& newUsername) {
    newUsers.insert(newUsername);
}

// Checks the users created this session.
bool Session::isNewUser(const std::string& checkUsername) const {
    return newUsers.count(checkUsername) > 0;
}

// Remembers the new game until logout.
void Session::markNewGame(const std::string& gameName) {
    newGames.insert(gameName);
}

// Checks the games put up for sale this session.
bool Session::isNewGame(const std::string& gameName) const {
    return newGames.count(gameName) > 0;
}

// Adds to the running total for this account.
void Session::recordCreditAdded(const std::string& accountName, long long cents) {
    creditAddedCents[accountName] += cents;
}

// Returns 0 for an account with no credit added yet.
long long Session::getCreditAddedCents(const std::string& accountName) const {
    auto found = creditAddedCents.find(accountName);
    return found == creditAddedCents.end() ? 0 : found->second;
}
