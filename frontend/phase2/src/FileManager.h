// FileManager.h
//
// FileManager
// The only class that touches files. It reads the three input files
// (current user accounts, available games, game collection) and appends the
// session's lines to the daily transaction file. It never changes the input
// files: updating those is the Back End's job.
//
// Each input file is fixed-width and ends with a line whose first field is END.
// Reading stops at that line. A missing file reads as empty and a line that
// can't be understood is skipped, so a bad file never crashes the program.
//
// File names are given on the command line and passed in by main.cpp.

#pragma once

#include <string>
#include <vector>

#include "Game.h"
#include "OwnedGame.h"
#include "User.h"

class FileManager {
public:
    // Creates a file manager for the given current user accounts, available
    // games and game collection files (read only) and daily transaction file
    // (appended to at each logout).
    FileManager(const std::string& usersFile, const std::string& gamesFile,
                const std::string& collectionFile, const std::string& transactionFile);

    // Reads every account from the current user accounts file, up to END.
    std::vector<User> readUsers() const;

    // Reads every game for sale from the available games file, up to END.
    std::vector<Game> readGames() const;

    // Reads every ownership record from the game collection file, up to END.
    std::vector<OwnedGame> readCollection() const;

    // Adds the given lines to the end of the daily transaction file, one per line.
    void appendTransactions(const std::vector<std::string>& lines) const;

private:
    // Returns the lines of a file before its END line, or nothing if the file can't be opened.
    // firstFieldWidth is the width of the field that holds END (username or game name).
    std::vector<std::string> readLinesUntilEnd(const std::string& fileName, std::size_t firstFieldWidth) const;

    std::string usersFile;        // current user accounts file
    std::string gamesFile;        // available games file
    std::string collectionFile;   // game collection file
    std::string transactionFile;  // daily transaction file
};
