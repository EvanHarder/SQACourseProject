/*
 * TransactionLog.h
 *
 * Class: TransactionLog
 * Intention: Collects the successful transactions of a session and writes
 * them to the daily transaction file at logout, for the Back End to process.
 */

#ifndef TRANSACTION_LOG_H
#define TRANSACTION_LOG_H

#include <cstddef>
#include <string>
#include <vector>

class TransactionLog {
private:
    std::vector<std::string> sessionTransactions;
    std::string dailyLogFilename;
    mutable std::size_t writtenTransactions = 0;

public:
    // Stores the path of the daily transaction file; nothing is written yet.
    explicit TransactionLog(std::string logFilename);

    void recordCreate(const std::string& username, const std::string& userType, double credit);
    void recordDelete(const std::string& username, const std::string& userType, double credit);
    void recordSell(const std::string& gameName, const std::string& seller, double price);
    void recordBuy(const std::string& gameName, const std::string& seller,
                   const std::string& buyer, double price);
    void recordRefund(const std::string& buyer, const std::string& seller, double credit);
    void recordAddCredit(const std::string& username, const std::string& userType, double credit);
    void recordEndOfSession(const std::string& username, const std::string& userType, double credit);

    // Appends newly recorded transactions; returns false if writing fails.
    bool outputTransactions() const;
};

#endif
