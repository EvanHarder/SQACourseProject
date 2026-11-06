/*
 * Session.h
 *
 * Class: Session
 * Intention: Keeps track of who is logged in during the current Front End
 * session (between a login and a logout).
 *
 * Also remembers what changed during the session that later transactions must
 * respect: users created or deleted, games put up for sale, and credit added per account.
 * All of it is cleared at logout, since the data files are reloaded at the next login.
 */

#ifndef SESSION_H
#define SESSION_H

#include <map>
#include <set>
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

    // Records that a user was deleted this session.
    void markDeleted(const std::string& deletedUsername);

    // Returns true if the user was deleted this session, so no further transactions may use them.
    bool isDeleted(const std::string& checkUsername) const;

    // Records a user created this session.
    void markNewUser(const std::string& newUsername);

    // Returns true if the user was created this session, so they cannot be used until the next session.
    bool isNewUser(const std::string& checkUsername) const;

    // Records a game put up for sale this session.
    void markNewGame(const std::string& gameName);

    // Returns true if the game was put up for sale this session, so it cannot be bought until the next session.
    bool isNewGame(const std::string& gameName) const;

    // Adds to the credit (in cents) added to an account this session.
    void recordCreditAdded(const std::string& accountName, long long cents);

    // Returns the credit (in cents) added to an account this session, for the $1000.00 limit.
    long long getCreditAddedCents(const std::string& accountName) const;

private:
    bool loggedIn;          // whether a user is currently logged in
    std::string username;   // the logged-in user's name
    std::string userType;   // the logged-in user's two-letter type code
    std::set<std::string> deletedUsers;                 // users deleted this session
    std::set<std::string> newUsers;                     // users created this session
    std::set<std::string> newGames;                     // games put up for sale this session
    std::map<std::string, long long> creditAddedCents;  // credit added per account this session, in cents
};

#endif