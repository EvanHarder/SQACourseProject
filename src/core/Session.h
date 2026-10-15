/*
 * Session.h
 *
 * Class: Session
 * Intention: Keeps track of who is logged in during the current Front End
 * session (between a login and a logout).
 *
 * First version: only the logged-in user. Later versions add what changed this
 * session (deleted users, games put up for sale, credit added per account).
 */

#ifndef SESSION_H
#define SESSION_H

#include <string>

class Session {
public:
    // Creates a session with nobody logged in.
    Session();

    // Records that username (with the given two-letter user type, e.g. "AA") has logged in.
    void start(const std::string& username, const std::string& userType);

    // Records that the current user has logged out.
    void end();

    // Returns true while a user is logged in.
    bool isLoggedIn() const;

    // Returns the username of the logged-in user ("" if nobody is logged in).
    const std::string& getUsername() const;

    // Returns the user type of the logged-in user: AA, FS, BS or SS ("" if nobody).
    const std::string& getUserType() const;

    // Returns true if the logged-in user is an admin (type AA).
    bool isAdmin() const;

private:
    bool loggedIn;          // whether a user is currently logged in
    std::string username;   // the logged-in user's name
    std::string userType;   // the logged-in user's two-letter type code
};

#endif