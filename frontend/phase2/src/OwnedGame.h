// OwnedGame.h
//
// OwnedGame
// One line of the game collection file: a record that a user owns a copy of
// a game. buy uses these to refuse a game the buyer already owns, and adds a
// new one after a successful purchase.
//
// It is separate from Game because a collection entry has an owner instead of
// a seller and no price. A record never changes once made, so it has no setters.

#pragma once

#include <string>

class OwnedGame {
public:
    // Creates a record that the given user owns a copy of the given game.
    OwnedGame(const std::string& gameName, const std::string& owner);

    // Returns the name of the owned game.
    std::string getGameName() const;

    // Returns the username of the user who owns the copy.
    std::string getOwner() const;

private:
    std::string gameName;  // name of the game
    std::string owner;     // username of the user who owns a copy
};
