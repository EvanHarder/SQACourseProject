// FileManager.cpp
//
// Method definitions for FileManager. See FileManager.h for what the class is for.

#include "FileManager.h"

#include <fstream>

#include "Formatter.h"

// Column where each field starts. Fields are separated by one space, so each
// field starts one column after the previous field ends:
//   start = previous start + previous width + 1
// Every file's first field (username or game name) starts at column 0.

// Current user accounts file:  UUUUUUUUUUUUUUU TT CCCCCCCCC
static const std::size_t USER_NAME_START   = 0;
static const std::size_t USER_TYPE_START   = USER_NAME_START + Config::USERNAME_WIDTH + 1;
static const std::size_t USER_CREDIT_START = USER_TYPE_START + Config::USER_TYPE_WIDTH + 1;

// Available games file:  IIIIIIIIIIIIIIIIIIIIIIIIII SSSSSSSSSSSSSSS PPPPPP
static const std::size_t GAME_NAME_START   = 0;
static const std::size_t GAME_SELLER_START = GAME_NAME_START + Config::GAME_NAME_WIDTH + 1;
static const std::size_t GAME_PRICE_START  = GAME_SELLER_START + Config::USERNAME_WIDTH + 1;

// Game collection file:  IIIIIIIIIIIIIIIIIIIIIIIIII SSSSSSSSSSSSSSS
static const std::size_t OWNED_GAME_START  = 0;
static const std::size_t OWNED_OWNER_START = OWNED_GAME_START + Config::GAME_NAME_WIDTH + 1;

// Creates a file manager for the given input files and daily transaction file.
FileManager::FileManager(const std::string& usersFile, const std::string& gamesFile,
                         const std::string& collectionFile, const std::string& transactionFile)
    : usersFile(usersFile), gamesFile(gamesFile),
      collectionFile(collectionFile), transactionFile(transactionFile) {}

// Reads every account from the current user accounts file, up to END.
// Lines with an unreadable credit amount are skipped.
std::vector<User> FileManager::readUsers() const {
    std::vector<User> users;
    for (const std::string& line : readLinesUntilEnd(usersFile, Config::USERNAME_WIDTH)) {
        std::string name = Formatter::extractField(line, USER_NAME_START, Config::USERNAME_WIDTH);
        std::string type = Formatter::extractField(line, USER_TYPE_START, Config::USER_TYPE_WIDTH);
        Cents credit = 0;
        if (Formatter::parseMoney(Formatter::extractField(line, USER_CREDIT_START, Config::CREDIT_WIDTH), credit)) {
            users.push_back(User(name, type, credit));
        }
    }
    return users;
}

// Reads every game for sale from the available games file, up to END.
// Lines with an unreadable price are skipped.
std::vector<Game> FileManager::readGames() const {
    std::vector<Game> games;
    for (const std::string& line : readLinesUntilEnd(gamesFile, Config::GAME_NAME_WIDTH)) {
        std::string name = Formatter::extractField(line, GAME_NAME_START, Config::GAME_NAME_WIDTH);
        std::string seller = Formatter::extractField(line, GAME_SELLER_START, Config::USERNAME_WIDTH);
        Cents price = 0;
        if (Formatter::parseMoney(Formatter::extractField(line, GAME_PRICE_START, Config::PRICE_WIDTH), price)) {
            games.push_back(Game(name, seller, price));
        }
    }
    return games;
}

// Reads every ownership record from the game collection file, up to END.
std::vector<OwnedGame> FileManager::readCollection() const {
    std::vector<OwnedGame> collection;
    for (const std::string& line : readLinesUntilEnd(collectionFile, Config::GAME_NAME_WIDTH)) {
        std::string gameName = Formatter::extractField(line, OWNED_GAME_START, Config::GAME_NAME_WIDTH);
        std::string owner = Formatter::extractField(line, OWNED_OWNER_START, Config::USERNAME_WIDTH);
        collection.push_back(OwnedGame(gameName, owner));
    }
    return collection;
}

// Adds the given lines to the end of the daily transaction file, one per line.
// Appending means a second session in the same run adds to the file instead of replacing it.
void FileManager::appendTransactions(const std::vector<std::string>& lines) const {
    std::ofstream file(transactionFile, std::ios::app);
    for (const std::string& line : lines) {
        file << line << "\n";
    }
}

// Returns the lines of a file before its END line, or nothing if the file can't be opened.
// Blank lines are skipped.
std::vector<std::string> FileManager::readLinesUntilEnd(const std::string& fileName,
                                                        std::size_t firstFieldWidth) const {
    std::vector<std::string> lines;
    std::ifstream file(fileName);
    std::string line;
    while (std::getline(file, line)) {
        // Compare the whole first field, so a real name that starts with "END " isn't mistaken for the end
        if (Formatter::extractField(line, 0, firstFieldWidth) == Config::END_MARKER) {
            break;
        }
        if (!Formatter::trimRight(line).empty()) {
            lines.push_back(line);
        }
    }
    return lines;
}
