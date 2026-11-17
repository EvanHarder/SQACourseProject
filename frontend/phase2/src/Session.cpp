// Session.cpp
//
// Method definitions for Session. See Session.h for what the class is for.

#include "Session.h"

// Creates a session with nobody logged in that reads and writes files through fileManager.
Session::Session(const FileManager& fileManager) : fileManager(fileManager), loggedIn(false) {}

// Reads the input files and logs in the given user.
// The accounts file is read first so the username can be checked; the games
// and collection files are only read once the username is accepted.
bool Session::login(const std::string& username) {
    clear();
    users = fileManager.readUsers();
    if (findUser(username) == nullptr) {
        users.clear();
        return false;
    }

    games = fileManager.readGames();
    collection = fileManager.readCollection();
    currentUsername = username;
    loggedIn = true;
    return true;
}

// Appends this session's recorded lines to the daily transaction file and logs out.
void Session::logout() {
    fileManager.appendTransactions(pendingTransactions);
    clear();
}

// Returns true between a successful login and the next logout.
bool Session::isLoggedIn() const {
    return loggedIn;
}

// Returns the logged-in user. The current user can't be deleted, so they are always found.
User& Session::getCurrentUser() {
    return *findUser(currentUsername);
}

// Returns the user with the given username, or nullptr if there is none.
User* Session::findUser(const std::string& username) {
    for (User& user : users) {
        if (user.getName() == username) {
            return &user;
        }
    }
    return nullptr;
}

// Returns every current account.
const std::vector<User>& Session::getUsers() const {
    return users;
}

// Adds a new account.
void Session::addUser(const User& user) {
    users.push_back(user);
}

// Removes the account with the given username and every game it has for sale.
void Session::removeUser(const std::string& username) {
    std::vector<User> remainingUsers;
    for (const User& user : users) {
        if (user.getName() != username) {
            remainingUsers.push_back(user);
        }
    }
    users = remainingUsers;

    std::vector<Game> remainingGames;
    for (const Game& game : games) {
        if (game.getSeller() != username) {
            remainingGames.push_back(game);
        }
    }
    games = remainingGames;
}

// Returns the game for sale with the given name, or nullptr if there is none.
Game* Session::findGame(const std::string& gameName) {
    for (Game& game : games) {
        if (game.getName() == gameName) {
            return &game;
        }
    }
    return nullptr;
}

// Returns every game for sale.
const std::vector<Game>& Session::getGames() const {
    return games;
}

// Returns every game for sale except the ones listed this session, which
// can't be bought until the next session.
std::vector<Game> Session::getGamesForSale() const {
    std::vector<Game> forSale;
    for (const Game& game : games) {
        if (!game.isListedThisSession()) {
            forSale.push_back(game);
        }
    }
    return forSale;
}

// Adds a game put up for sale.
void Session::addGame(const Game& game) {
    games.push_back(game);
}

// Returns true if the given user owns a copy of the given game.
bool Session::ownsGame(const std::string& username, const std::string& gameName) const {
    for (const OwnedGame& owned : collection) {
        if (owned.getOwner() == username && owned.getGameName() == gameName) {
            return true;
        }
    }
    return false;
}

// Records that the given user now owns a copy of the given game.
void Session::addToCollection(const std::string& username, const std::string& gameName) {
    collection.push_back(OwnedGame(gameName, username));
}

// Returns how much credit has been added to the given account this session (0 if none).
Cents Session::getCreditAddedThisSession(const std::string& username) const {
    auto found = creditAddedThisSession.find(username);
    if (found == creditAddedThisSession.end()) {
        return 0;
    }
    return found->second;
}

// Adds amount to the given account's total for this session.
void Session::recordCreditAdded(const std::string& username, Cents amount) {
    creditAddedThisSession[username] += amount;
}

// Saves a daily transaction file line to be written at logout.
void Session::recordTransaction(const std::string& line) {
    pendingTransactions.push_back(line);
}

// Forgets everything from the current session.
void Session::clear() {
    loggedIn = false;
    currentUsername = "";
    users.clear();
    games.clear();
    collection.clear();
    creditAddedThisSession.clear();
    pendingTransactions.clear();
}
