/*
 * AvailableGames.cpp
 *
 * Implementation of the AvailableGames class: loads, searches and saves the
 * available games file. See AvailableGames.h for each method.
 *
 * File format (fixed width, one game per line, ends with an END line):
 *     game name (27) | seller (16) | price (6)
 *     Stardew Valley             sellUser        015.00
 */

#include "AvailableGames.h"
#include "FormatUtils.h"
#include <fstream>

// Stores the path; nothing is read until loadFromFile().
AvailableGames::AvailableGames(std::string gamesFilename) : filename(std::move(gamesFilename)) {}

// Reads every game line up to END; game and seller names may contain spaces.
bool AvailableGames::loadFromFile() {
    games.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (FormatUtils::trimText(line) == "END") break;
        if (line.length() < 49) continue;

        std::string name = FormatUtils::trimText(line.substr(0, 27));
        std::string seller = FormatUtils::trimText(line.substr(27, 16));
        double price = FormatUtils::textToMoney(line.substr(43, 6));
        games.push_back({seller, name, price});
    }
    return true;
}

// Writes every game in the fixed-width format, followed by the END line.
bool AvailableGames::saveToFile() {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) return false;

    for (const auto& game : games) {
        std::string pName = FormatUtils::padText(game.name, 27);
        std::string pSeller = FormatUtils::padText(game.seller, 16);
        std::string pPrice = FormatUtils::moneyToText(game.price, 6);
        file << pName << pSeller << pPrice << "\n";
    }
    file << FormatUtils::padText("END", 49) << "\n";

    return true;
}

// Returns the game with this exact name, or nullptr if it is not for sale.
const Game* AvailableGames::findGameByName(const std::string& name) const {
    for (const auto& game : games) {
        if (game.name == name) {
            return &game;
        }
    }
    return nullptr;
}

// Returns every game for sale (used by list).
const std::vector<Game>& AvailableGames::getAllGames() const {
    return games;
}