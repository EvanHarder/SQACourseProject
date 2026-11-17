// FrontEnd.cpp
//
// Method definitions for FrontEnd. See FrontEnd.h for what the class is for.

#include "FrontEnd.h"

#include "AddCreditTransaction.h"
#include "BuyTransaction.h"
#include "CreateTransaction.h"
#include "DeleteTransaction.h"
#include "ListAccountsTransaction.h"
#include "ListGamesTransaction.h"
#include "LoginTransaction.h"
#include "LogoutTransaction.h"
#include "RefundTransaction.h"
#include "SearchTransaction.h"
#include "SellTransaction.h"

// Creates a Front End that reads commands from input, prints to output and
// uses the files named in fileManager, with every transaction ready to run.
// Each new transaction class is added here.
FrontEnd::FrontEnd(std::istream& input, std::ostream& output, const FileManager& fileManager)
    : console(input, output), session(fileManager) {
    addTransaction(std::make_unique<LoginTransaction>());
    addTransaction(std::make_unique<LogoutTransaction>());
    addTransaction(std::make_unique<CreateTransaction>());
    addTransaction(std::make_unique<DeleteTransaction>());
    addTransaction(std::make_unique<SellTransaction>());
    addTransaction(std::make_unique<BuyTransaction>());
    addTransaction(std::make_unique<RefundTransaction>());
    addTransaction(std::make_unique<AddCreditTransaction>());
    addTransaction(std::make_unique<ListGamesTransaction>());
    addTransaction(std::make_unique<ListAccountsTransaction>());
    addTransaction(std::make_unique<SearchTransaction>());
}

// Reads and processes commands until the input runs out.
void FrontEnd::run() {
    while (true) {
        std::string commandName = console.prompt("Enter command:");
        if (!console.hasInput()) {
            break;
        }
        processCommand(commandName);
    }
}

// Makes the given transaction available under its command word.
void FrontEnd::addTransaction(std::unique_ptr<Transaction> transaction) {
    std::string commandName = transaction->getName();
    transactions[commandName] = std::move(transaction);
}

// Runs the shared checks for one command word, then runs its transaction.
void FrontEnd::processCommand(const std::string& commandName) {
    Transaction* transaction = findTransaction(commandName);
    if (transaction == nullptr) {
        console.printError("invalid command.");
        return;
    }
    if (transaction->requiresLogin() && !session.isLoggedIn()) {
        console.printError("you must log in first.");
        return;
    }
    if (session.isLoggedIn() && !transaction->isAllowedFor(session.getCurrentUser())) {
        console.printError("permission denied.");
        return;
    }
    transaction->execute(session, console);
}

// Returns the transaction for a command word, or nullptr if there is none.
Transaction* FrontEnd::findTransaction(const std::string& commandName) const {
    auto found = transactions.find(commandName);
    if (found == transactions.end()) {
        return nullptr;
    }
    return found->second.get();
}
