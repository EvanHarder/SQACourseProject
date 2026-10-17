/*
 * FrontEnd.h
 *
 * Class: FrontEnd
 * Intention: Runs one Front End execution: reads transaction codes from the
 * input stream one at a time and passes each one to the TransactionHandler,
 * until the input ends.
 *
 * main.cpp depends on the constructor and run(); keep these two signatures.
 */

#ifndef FRONT_END_H
#define FRONT_END_H

#include <string>

#include "core/Session.h"
#include "data/AvailableGames.h"
#include "data/GameCollection.h"
#include "data/TransactionLog.h"
#include "data/UserAccounts.h"
#include "io/InputReader.h"
#include "io/ResultPrinter.h"
#include "transactions/TransactionHandler.h"

class FrontEnd {
public:
    // Creates the printer, session, data objects and transaction handler for the four files.
    FrontEnd(const std::string& accountsFilePath, const std::string& availableGamesFilePath, const std::string& gameCollectionFilePath, const std::string& dailyTransactionFilePath, InputReader& inputReader);

    // Processes transactions until the end of input. Never stops early on bad input.
    void run();

private:
    // Members are built in this order; the handler must come last because it uses the others.
    InputReader& reader;            // source of "label: value" transaction lines
    ResultPrinter printer;          // prints result blocks to standard output
    Session session;                // who is logged in
    UserAccounts accounts;          // current user accounts file
    AvailableGames games;           // available games file
    GameCollection collection;      // game collection file
    TransactionLog log;             // daily transaction file, written at logout
    TransactionHandler handler;     // carries out each transaction
};

#endif