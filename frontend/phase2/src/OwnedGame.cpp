// OwnedGame.cpp
//
// Method definitions for OwnedGame. See OwnedGame.h for what the class is for.

#include "OwnedGame.h"

// Creates a record that the given user owns a copy of the given game.
OwnedGame::OwnedGame(const std::string& gameName, const std::string& owner)
    : gameName(gameName), owner(owner) {}

// Returns the name of the owned game.
std::string OwnedGame::getGameName() const {
    return gameName;
}

// Returns the username of the user who owns the copy.
std::string OwnedGame::getOwner() const {
    return owner;
}
