/*
 * FrontEnd.h
 *
 * Class: FrontEnd
 * Intention: Runs one Front End execution: loads the data files, reads
 * transactions from the input stream one at a time, and passes each one to
 * the right transaction method until the input ends.
 *
 * AGREED INTERFACE: main.cpp depends on the constructor and run() below.
 * The owner of FrontEnd adds the private members and writes FrontEnd.cpp,
 * but should keep these two public signatures unchanged.
 *
 * Owner: (to be assigned)
 */

#ifndef FRONT_END_H
#define FRONT_END_H

#include <string>
#include "io/InputReader.h"

class FrontEnd {
public:
    // Stores the four file paths and the reader used for the transaction stream.
    FrontEnd(const std::string& accountsFilePath,
             const std::string& availableGamesFilePath,
             const std::string& gameCollectionFilePath,
             const std::string& dailyTransactionFilePath,
             InputReader& inputReader);

    // Processes transactions until the end of input. Never stops early on bad input.
    void run();

private:
    std::string accountsFile;           // current user accounts file (input)
    std::string availableGamesFile;     // available games file (input)
    std::string gameCollectionFile;     // game collection file (input)
    std::string dailyTransactionFile;   // daily transaction file (output, written at logout)
    InputReader& reader;                // source of "label: value" transaction lines
};

#endif
