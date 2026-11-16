/*
 * TransactionHandler.h
 *
 * Class: TransactionHandler
 * Intention: Carries out each transaction of the Digital Games Distribution
 * System: checks the user is logged in and allowed, reads the transaction's
 * fields, checks its rules, applies the change, records it for the daily
 * transaction file and prints the result block.
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
    // Connects the handler to the existing input, output, and session objects.
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
    InputReader& inputReader;        // source of the transaction's fields
    ResultPrinter& resultPrinter;    // prints the result block
    Session& session;                // who is logged in and what changed this session
    UserAccounts& userAccounts;      // current user accounts
    AvailableGames& availableGames;  // games for sale
    GameCollection& gameCollection;  // games each user owns
    TransactionLog& transactionLog;  // lines for the daily transaction file

    // Returns true if the logged-in user's type is allowed to run the transaction.
    bool isPermitted(const std::string& transactionCode) const;

    // login: reads the username, loads the data files and starts the session.
    void handleLogin();
    // logout: writes the daily transaction file and ends the session.
    void handleLogout();
    // create (admin): reads username, email, password and type; records a new user.
    void handleCreate();
    // delete (admin): reads a username; removes the user and their games for sale.
    void handleDelete();
    // sell: reads a game name and price; puts the game up for sale from the next session.
    void handleSell();
    // buy: reads a game name and seller; buys the game for the logged-in user.
    void handleBuy();
    // refund (admin): reads buyer, seller and amount; moves the credit.
    void handleRefund();
    // addcredit: reads an amount (and a username for admins); adds credit within the $1000 session limit.
    void handleAddCredit();
    // list: prints every game for sale.
    void handleList();
    // listaccounts (admin): prints every active account with its type and credit.
    void handleListAccounts();
    // search: prints every game and user whose name contains the query.
    void handleSearch();

    // Prints a temporary result for unfinished handlers.
    void printNotImplemented(const std::string& transactionCode);
};

#endif 
