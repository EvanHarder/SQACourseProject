/*
 * TransactionLog.h
 *
 * Class: TransactionLog
 * Intention: Collects the successful transactions of a session and writes
 * them to the daily transaction file at logout, for the Back End to process.
 */

#ifndef TRANSACTION_LOG_H
#define TRANSACTION_LOG_H

#include <string>
#include <vector>

// One successful transaction waiting to be written to the daily transaction file.
struct Transaction {
    std::string username;   // user the transaction belongs to
    std::string gameName;   // game involved, if any
    double amount;          // money involved in dollars
};

class TransactionLog {
private:
    std::vector<Transaction> sessionTransactions;   // transactions recorded this session, in order
    std::string dailyLogFilename;                   // path of the daily transaction file

public:
    // Stores the path of the daily transaction file; nothing is written yet.
    explicit TransactionLog(std::string logFilename);

    // Adds one successful transaction to the list for this session.
    void recordTransaction(const std::string& username, const std::string& gameName, double amount);

    // Appends every recorded transaction to the daily transaction file; returns false if it cannot be opened.
    bool outputTransactions() const;
};

#endif