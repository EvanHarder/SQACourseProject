#include "AvailableGames.h"
#include "FormatUtils.h"
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
        std::string seller, name;
        double price;
        if (iss >> name >> seller >> price) {
            games.push_back({name, seller, price});
        }
    }
    return true;

}
bool AvailableGames::saveToFile() {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) return false;

    for (const auto& game : games) {
    std::string pName = FormatUtils::padText(game.name, 27);
    std::string pSeller = FormatUtils::padText(game.seller, 16);
    std::string pPrice = FormatUtils::moneyToText(game.price, 6);
    file << pName << pSeller << pPrice << "\n";
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