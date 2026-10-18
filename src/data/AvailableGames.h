/*
 * AvailableGames.h
 *
 * Class: AvailableGames
 * Intention: Loads the available games file and finds games for sale by name.
 * Each game is stored as a Game record.
 */

#ifndef AVAILABLE_GAMES_H
#define AVAILABLE_GAMES_H

#include <string>
#include <vector>

// One game for sale from the available games file.
struct Game {
    std::string seller;     // username of the seller
    std::string name;       // game name, up to 25 characters, may contain spaces
    double price;           // price in dollars, at most 999.99
};

class AvailableGames {
private:
    std::vector<Game> games;    // every game loaded from the file
    std::string filename;       // path of the available games file

public:
    // Stores the path of the games file; nothing is read yet.
    explicit AvailableGames(std::string gamesFilename);

    // Reads every game from the file; returns false if the file cannot be opened.
    bool loadFromFile();

    // Writes every game back to the file in the fixed-width format; returns false if it cannot be written.
    bool saveToFile();

    // Returns the game with the given name, or nullptr if it is not for sale.
    const Game* findGameByName(const std::string& name) const;

    // Returns every game for sale.
    const std::vector<Game>& getAllGames() const;
};

#endif