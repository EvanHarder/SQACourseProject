/*
 * main.cpp
 * Digital Games Distribution System - Front End (CSCI 3060U course project)
 *
 * PROGRAM INTENTION
 *   The Front End is a console point-of-purchase terminal for a digital game store.
 *   It processes one session of transactions (login, logout, create, delete, sell,
 *   buy, refund, addcredit, list, listaccounts) and records every successful change
 *   in a daily transaction file that the Back End processes overnight.
 *
 * INPUT FILES (read at startup)
 *   1. Current user accounts file   - every user, their type and their credit
 *   2. Available games file         - every game for sale, its seller and price
 *   3. Game collection file         - which games each user already owns
 *
 * INPUT STREAM (standard input)
 *   One "label: value" line per entry, for example:
 *       transaction: login
 *       username: admin01
 *
 * When the input stream is typed at a keyboard (not redirected from a file), a
 * welcome screen listing the transactions and their fields is shown first.
 * It is never shown when input comes from a file, so test outputs are unchanged.
 *
 * OUTPUTS
 *   - Standard output: one result block per transaction (key value, Status,
 *     Message and outcome lines)
 *   - Daily transaction file: written at each logout, one fixed-width line per
 *     successful transaction, ending with a 00 end-of-session line
 *
 * HOW TO RUN
 *   frontend <accounts_file> <available_games_file> <game_collection_file> <daily_transaction_file> < input.txt
 *
 *   Example (from the repository root):
 *   ./frontend TestCases/data/current_user_accounts.txt TestCases/data/available_games.txt TestCases/data/game_collection.txt daily_transaction.txt < TestCases/login/success/login01/input.txt
 */

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "core/FrontEnd.h"
#include "io/InputReader.h"

namespace {
    // Positions of the command-line arguments (argv[0] is the program name).
    const int ACCOUNTS_ARGUMENT = 1;
    const int AVAILABLE_GAMES_ARGUMENT = 2;
    const int GAME_COLLECTION_ARGUMENT = 3;
    const int DAILY_TRANSACTION_ARGUMENT = 4;
    const int EXPECTED_ARGUMENT_COUNT = 5;

    // Prints how to run the program to standard error.
    void printUsage(const std::string& programName) {
        std::cerr << "Usage: " << programName << " <accounts_file> <available_games_file> <game_collection_file> <daily_transaction_file> < input.txt" << std::endl;
    }

    // Returns true when standard input is a keyboard rather than a redirected file or pipe.
    bool inputIsKeyboard() {
#ifdef _WIN32
        return _isatty(_fileno(stdin)) != 0;
#else
        return isatty(fileno(stdin)) != 0;
#endif
    }

    // Prints the welcome screen: how to enter transactions and the fields each one asks for.
    void printWelcomeScreen() {
        std::cout
            << "==============================================================================\n"
            << "  Digital Games Distribution System\n"
            << "==============================================================================\n"
            << "Type one \"label: value\" line at a time. Start by logging in:\n"
            << "    transaction: login\n"
            << "    username: admin01\n"
            << "\n"
            << "  Transaction    Fields it asks for (in this order)               Who can use it\n"
            << "  -------------  -----------------------------------------------  ---------------\n"
            << "  login          username                                         anyone\n"
            << "  logout         (none) - writes the daily transaction file       logged-in user\n"
            << "  create         username, email, password, type (AA/FS/BS/SS)    admin\n"
            << "  delete         username                                         admin\n"
            << "  sell           game, price                                      all but BS\n"
            << "  buy            game, seller                                     all but SS\n"
            << "  refund         buyer, seller, amount                            admin\n"
            << "  addcredit      amount (admins: then username)                   logged-in user\n"
            << "  list           (none)                                           logged-in user\n"
            << "  listaccounts   (none)                                           admin\n"
            << "  search         query                                            logged-in user\n"
            << "\n"
            << "Example:  transaction: buy   then   game: Hades   then   seller: fullUser\n"
            << "Finish with Ctrl+Z then Enter (Windows) or Ctrl+D (Linux/Mac).\n"
            << "==============================================================================\n"
            << std::endl;
    }

    // Returns true if the file at path exists and can be opened for reading.
    bool isReadableFile(const std::string& path) {
        std::ifstream file(path);
        return file.good();
    }
}

// Checks the command-line arguments, then runs one Front End session.
// Returns 0 when the session finishes, or 1 if the program cannot start.
int main(int argc, char* argv[]) {
    if (argc != EXPECTED_ARGUMENT_COUNT) {
        printUsage(argv[0]);
        return 1;
    }

    std::string accountsFile = argv[ACCOUNTS_ARGUMENT];
    std::string availableGamesFile = argv[AVAILABLE_GAMES_ARGUMENT];
    std::string gameCollectionFile = argv[GAME_COLLECTION_ARGUMENT];
    std::string dailyTransactionFile = argv[DAILY_TRANSACTION_ARGUMENT];

    for (const std::string& inputFile : {accountsFile, availableGamesFile, gameCollectionFile}) {
        if (!isReadableFile(inputFile)) {
            std::cerr << "Error: cannot open input file " << inputFile << std::endl;
            return 1;
        }
    }

    if (inputIsKeyboard()) {
        printWelcomeScreen();
    }

    InputReader inputReader(std::cin);
    FrontEnd frontEnd(accountsFile, availableGamesFile, gameCollectionFile, dailyTransactionFile, inputReader);
    frontEnd.run();
    return 0;
}