// Game.cpp
//
// Method definitions for Game. See Game.h for what the class is for.

#include "Game.h"

// Creates a game with the given name, seller username, price in cents and
// whether it was listed this session.
Game::Game(const std::string& name, const std::string& seller, Cents price, bool listedThisSession)
    : name(name), seller(seller), price(price), listedThisSession(listedThisSession) {}

// Returns the game's name.
std::string Game::getName() const {
    return name;
}

// Returns the username of the user selling the game.
std::string Game::getSeller() const {
    return seller;
}

// Returns the price in cents.
Cents Game::getPrice() const {
    return price;
}

// Returns true if the game was put up for sale this session.
bool Game::isListedThisSession() const {
    return listedThisSession;
}
