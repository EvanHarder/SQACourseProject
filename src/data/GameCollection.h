//Used for file reading and searching for personal game libary
#ifndef GAME_COLLECTION_H
#define GAME_COLLECTION_H

#include <string>
#include <vector>
#include <unordered_map>

class GameCollection {
private:
    // Maps username -> list of owned game IDs/names
    std::unordered_map<std::string, std::vector<std::string>> userGames;
    std::string filename;

public:
    explicit GameCollection(std::string collectionFilename);
    
    //Loads from a text file into the hashmap userGames
    bool loadFromFile();
    bool userOwnsGame(const std::string& username, const std::string& gameName) const;
    void addGameToUser(const std::string& username, const std::string& gameName);
};

#endif