/*
 * GameCollection.h
 *
 * Class: GameCollection
 * Intention: Loads the game collection file and answers whether a user
 * already owns a game.
 */

#ifndef GAME_COLLECTION_H
#define GAME_COLLECTION_H

#include <string>
#include <unordered_map>
#include <vector>

class GameCollection {
private:
    std::unordered_map<std::string, std::vector<std::string>> userGames;    // username -> names of the games they own
    std::string filename;                                                    // path of the game collection file

public:
    // Stores the path of the collection file; nothing is read yet.
    explicit GameCollection(std::string collectionFilename);

    // Reads every owned game from the file; returns false if the file cannot be opened.
    bool loadFromFile();

    // Writes every owned game back to the file in the fixed-width format; returns false if it cannot be written.
    bool saveToFile();

    // Returns true if the user already owns a game with this name.
    bool userOwnsGame(const std::string& username, const std::string& gameName) const;

    // Records that the user now owns the game (in memory only).
    void addGameToUser(const std::string& username, const std::string& gameName);
};

#endif