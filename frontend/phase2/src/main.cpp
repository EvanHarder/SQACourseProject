// main.cpp
//
// Front End for the Digital Games Distribution System (CSCI 3060U).
// A point-of-purchase terminal: reads transactions (login, logout, create,
// delete, sell, buy, refund, addcredit, list, listaccounts, search) one line at a
// time from standard input and prints prompts and results to standard output.
//
// The four files are given on the command line, in this order:
//   1. current user accounts file   (read at every login, never changed)
//   2. available games file         (read at every login, never changed)
//   3. game collection file         (read at every login, never changed)
//   4. daily transaction file       (appended to at every logout)
//
// How to run:   make            (in phase2/, builds ./frontend)
//               ./frontend users.txt games.txt collection.txt transout.atf
//               ./frontend users.txt games.txt collection.txt transout.atf < session.inp

#include <iostream>

#include "FileManager.h"
#include "FrontEnd.h"

// Number of command-line arguments expected after the program name.
static const int FILE_ARGUMENT_COUNT = 4;

// Checks the command line, then runs the Front End on the terminal until the input ends.
int main(int argc, char* argv[]) {
    if (argc != FILE_ARGUMENT_COUNT + 1) {
        std::cerr << "Usage: " << argv[0]
                  << " <current user accounts file> <available games file>"
                  << " <game collection file> <daily transaction file>\n";
        return 1;
    }

    FileManager fileManager(argv[1], argv[2], argv[3], argv[4]);
    FrontEnd frontEnd(std::cin, std::cout, fileManager); // reads keyboard, prints to screen
    frontEnd.run(); // loop until input runs out
    return 0;
}
