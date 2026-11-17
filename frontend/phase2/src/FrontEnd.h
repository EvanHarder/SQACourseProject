// FrontEnd.h
//
// FrontEnd
// Runs the Front End: reads one command at a time until the input runs out,
// finds the transaction with that command word and runs it. Before running a
// transaction it makes the checks every command shares, in this order:
//   1. unknown command                      -> "invalid command."
//   2. needs a login but nobody is logged in -> "you must log in first."
//   3. user's type isn't allowed to run it   -> "permission denied."
// Doing these here means no transaction repeats them.
//
// FrontEnd owns the Console (all terminal input and output), the Session (all
// data between login and logout) and one object for each transaction.

#pragma once

#include <istream>
#include <map>
#include <memory>
#include <ostream>
#include <string>

#include "Console.h"
#include "Session.h"
#include "Transaction.h"

class FrontEnd {
public:
    // Creates a Front End that reads commands from input, prints to output and
    // uses the files named in fileManager, with every transaction ready to run.
    FrontEnd(std::istream& input, std::ostream& output, const FileManager& fileManager);

    // Reads and processes commands until the input runs out.
    void run();

private:
    // Makes the given transaction available under its command word.
    void addTransaction(std::unique_ptr<Transaction> transaction);

    // Runs the shared checks for one command word, then runs its transaction.
    void processCommand(const std::string& commandName);

    // Returns the transaction for a command word, or nullptr if there is none.
    Transaction* findTransaction(const std::string& commandName) const;

    Console console;   // all keyboard input and screen output
    Session session;   // everything known between login and logout
    std::map<std::string, std::unique_ptr<Transaction>> transactions;  // command word -> transaction
};
