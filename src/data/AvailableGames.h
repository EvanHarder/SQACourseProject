//Used for file reading and searching from total game libary
#ifndef AVAILABLE_GAMES_H
#define AVAILABLE_GAMES_H

#include <string>
#include <vector>

struct Game {
    std::string id;
    std::string name;
    double price;
};

class AvailableGames {
private:
    std::vector<Game> games;
    std::string filename;

public:
    explicit AvailableGames(std::string gamesFilename);
    //Loads into the getAllGames getter
    bool loadFromFile();
    //Search
    const Game* findGameByName(const std::string& name) const;
    const std::vector<Game>& getAllGames() const;
};

#endif