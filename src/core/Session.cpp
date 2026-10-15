/*
 * Session.cpp
 *
 * Implementation of the Session class: tracks the logged-in user.
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

// Clears the logged-in user.
void Session::end() {
    loggedIn = false;
    username.clear();
    userType.clear();
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