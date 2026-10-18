/*
 * TransactionLog.cpp
 *
 * Implementation of the TransactionLog class: stores the session's successful
 * transactions and writes them to the daily transaction file.
 * See TransactionLog.h for the description of each method.
 */

#include "TransactionLog.h"
#include "FormatUtils.h"
#include <fstream>

// Stores the path; nothing is written until outputTransactions().
TransactionLog::TransactionLog(std::string logFilename) : dailyLogFilename(std::move(logFilename)) {}

// Keeps the transaction in memory until logout.
void TransactionLog::recordTransaction(const std::string& username, const std::string& gameName, double amount) {
    sessionTransactions.push_back({username, gameName, amount});
}

// Appends one padded line per transaction to the daily transaction file.
// First version: the line layout does not match the required transaction codes yet (finished in Phase 3).
bool TransactionLog::outputTransactions() const {
    std::ofstream file(dailyLogFilename, std::ios::app);
    if (!file.is_open()) return false;

    for (const auto& tx : sessionTransactions) {
        std::string pUser = FormatUtils::padText(tx.username, 15);
        std::string pGame = FormatUtils::padText(tx.gameName, 20);
        std::string pAmount = FormatUtils::moneyToText(tx.amount, 10);

        file << pUser << pGame << pAmount << "\n";
    }
    return true;
}