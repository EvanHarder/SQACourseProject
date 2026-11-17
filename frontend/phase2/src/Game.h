// Game.h
//
// Game
// One game for sale, from the available games file (or one put up for sale
// by sell this session): the game's name, the seller's username and the price.
//
// Nothing in the Front End changes a game once it is listed (there is no
// transaction to rename a game, change its price or change its seller), so it
// has no setters. A game listed this session is marked so buy can refuse it:
// the spec says a new game can't be traded until the next session.

#pragma once

#include <string>

#include "Constants.h"

class Game {
public:
    // Creates a game with the given name, seller username and price in cents.
    // listedThisSession is true only for games put up for sale by sell in this session.
    Game(const std::string& name, const std::string& seller, Cents price, bool listedThisSession = false);

    // Returns the game's name.
    std::string getName() const;

    // Returns the username of the user selling the game.
    std::string getSeller() const;

    // Returns the price in cents.
    Cents getPrice() const;

    // Returns true if the game was put up for sale this session (so it can't be bought yet).
    bool isListedThisSession() const;

private:
    std::string name;        // the game's name, at most Config::MAX_GAME_NAME_LENGTH characters
    std::string seller;      // username of the user selling it
    Cents price;             // price in cents, at most Config::MAX_PRICE
    bool listedThisSession;  // true if sell added it during this session
};
