/*
 * TransactionHandler.h
 *
 * Class: TransactionHandler
 * Intention: Handles different types of transactions in the Digital Games Distribution System.
 */



#ifndef TRANSACTION_HANDLER_H
#define TRANSACTION_HANDLER_H

#include <string>

#include "core/Session.h"
#include "io/InputReader.h"
#include "io/ResultPrinter.h"
#include "data/UserAccounts.h"
#include "data/AvailableGames.h"
#include "data/GameCollection.h"
#include "data/TransactionLog.h"   

// Handles transactions requested by the Front End.
class TransactionHandler {
public:
    // Connects the handler to the existing input, output,
    // and session objects.
    TransactionHandler(InputReader& reader,
                       ResultPrinter& printer,
                       Session& currentSession,
                       UserAccounts& accounts,
                       AvailableGames& games,
                       GameCollection& collection,
                       TransactionLog& log
                       );

    // Identifies a transaction and calls its handler.
    void handleTransaction(const std::string& transactionCode);

private:
    InputReader& inputReader;
    ResultPrinter& resultPrinter;
    Session& session;
    UserAccounts& userAccounts;
    AvailableGames& availableGames;
    GameCollection& gameCollection;
    TransactionLog& transactionLog;

    bool isPermitted(const std::string& transactionCode) const;

    // Handles individual transaction types.
    void handleLogin();
    void handleLogout();
    void handleCreate();
    void handleDelete();
    void handleSell();
    void handleBuy();
    void handleRefund();
    void handleAddCredit();
    void handleList(); //list games 
    void handleListAccounts(); //list accounts

    // Prints a temporary result for unfinished handlers.
    void printNotImplemented(const std::string& transactionCode);
};

#endif // TRANSACTION_HANDLER_H
