// Session.h
//
// Session
// Everything the Front End knows between a login and a logout: who is logged
// in, the users, games for sale and game collection (including changes made
// this session), how much credit each account has been given this session,
// and the transaction lines waiting to be written.
//
// Every login re-reads the three input files. The Front End never changes
// those files (the Back End applies the day's transactions overnight), so a
// new session always starts from the same saved state and forgets the
// previous session's in-memory changes. Those changes only survive as lines
// in the daily transaction file, written at logout.
//
// Pointers returned by findUser, getCurrentUser and findGame stay valid only
// until the next change to the users or games (addUser, removeUser, addGame),
// so callers use them right away and don't keep them.

#pragma once

#include <map>
#include <string>
#include <vector>

#include "Constants.h"
#include "FileManager.h"
#include "Game.h"
#include "OwnedGame.h"
#include "User.h"

class Session {
public:
    // Creates a session with nobody logged in that reads and writes files through fileManager.
    explicit Session(const FileManager& fileManager);

    // ---- Logging in and out ----

    // Reads the input files and logs in the given user.
    // Returns false (and stays logged out) if no account has that username.
    bool login(const std::string& username);

    // Appends this session's recorded lines to the daily transaction file and logs out.
    void logout();

    // Returns true between a successful login and the next logout.
    bool isLoggedIn() const;

    // Returns the logged-in user. Only call while logged in.
    User& getCurrentUser();

    // ---- Users ----

    // Returns the user with the given username, or nullptr if there is none.
    User* findUser(const std::string& username);

    // Returns every current account, in file order with new accounts last.
    const std::vector<User>& getUsers() const;

    // Adds a new account (made by create).
    void addUser(const User& user);

    // Removes the account with the given username and every game it has for sale.
    void removeUser(const std::string& username);

    // ---- Games for sale ----

    // Returns the game for sale with the given name, or nullptr if there is none.
    Game* findGame(const std::string& gameName);

    // Returns every game for sale, in file order with games listed this session last.
    const std::vector<Game>& getGames() const;

    // Returns the games that can be bought right now: every game for sale except
    // the ones listed this session (used by list and search).
    std::vector<Game> getGamesForSale() const;

    // Adds a game put up for sale (made by sell).
    void addGame(const Game& game);

    // ---- Game collection ----

    // Returns true if the given user owns a copy of the given game.
    bool ownsGame(const std::string& username, const std::string& gameName) const;

    // Records that the given user now owns a copy of the given game.
    void addToCollection(const std::string& username, const std::string& gameName);

    // ---- Per-session credit limit ----

    // Returns how much credit (in cents) has been added to the given account this session.
    Cents getCreditAddedThisSession(const std::string& username) const;

    // Adds amount (in cents) to the given account's total for this session.
    void recordCreditAdded(const std::string& username, Cents amount);

    // ---- Daily transaction file ----

    // Saves a daily transaction file line to be written at logout.
    void recordTransaction(const std::string& line);

private:
    // Forgets everything from the current session.
    void clear();

    FileManager fileManager;                               // reads the input files, writes the transaction file
    bool loggedIn;                                         // true between login and logout
    std::string currentUsername;                           // the logged-in account
    std::vector<User> users;                               // accounts, as changed so far this session
    std::vector<Game> games;                               // games for sale, as changed so far this session
    std::vector<OwnedGame> collection;                     // who owns which game
    std::map<std::string, Cents> creditAddedThisSession;   // per-account total, for the $1000 limit
    std::vector<std::string> pendingTransactions;          // lines to write at logout
};
