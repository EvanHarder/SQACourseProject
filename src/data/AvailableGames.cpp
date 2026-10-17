#include "AvailableGames.h"
#include <fstream>
#include <sstream>

AvailableGames::AvailableGames(std::string gamesFilename) : filename(std::move(gamesFilename)) {}

bool AvailableGames::loadFromFile() {
    games.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string id, name;
        double price;
        if (iss >> id >> name >> price) {
            games.push_back({id, name, price});
        }
    }
    return true;
}

const Game* AvailableGames::findGameByName(const std::string& name) const {
    for (const auto& game : games) {
        if (game.name == name) {
            return &game;
        }
    }
    return nullptr;
}

const std::vector<Game>& AvailableGames::getAllGames() const {
    return games;
}