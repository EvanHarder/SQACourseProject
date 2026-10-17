#include "GameCollection.h"
#include "FormatUtils.h"
#include <fstream>
#include <sstream>

GameCollection::GameCollection(std::string collectionFilename) : filename(std::move(collectionFilename)) {}

bool GameCollection::loadFromFile() {
    userGames.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string username, gameName;

        //ASSUMES ONE WORD GAMENAMES
        if (iss >> username >> gameName) {
            userGames[username].push_back(gameName);
        }
    }
    return true;
}

bool GameCollection::saveToFile() {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) return false;

    for (const auto& entry : userGames) {
        const std::string& username = entry.first;
        const std::vector<std::string>& gamesList = entry.second;

        std::string pUser = FormatUtils::padText(username, 15);

        
        for (const auto& gameTitle : gamesList) {
            
            std::string pGame = FormatUtils::padText(gameTitle, 26);

            
            file << pGame << pUser << "\n";
        }
    }

    return true;
}

bool GameCollection::userOwnsGame(const std::string& username, const std::string& gameName) const {
    auto it = userGames.find(username);
    if (it != userGames.end()) {
        for (const auto& g : it->second) {
            if (g == gameName) return true;
        }
    }
    return false;
}

void GameCollection::addGameToUser(const std::string& username, const std::string& gameName) {
    //Doesn't write to txt file
    userGames[username].push_back(gameName);
}