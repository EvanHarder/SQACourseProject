#include "TransactionLog.h"
#include "FormatUtils.h"
#include <fstream>

TransactionLog::TransactionLog(std::string logFilename) : dailyLogFilename(std::move(logFilename)) {}

void TransactionLog::recordTransaction(const std::string& username, const std::string& gameName, double amount) {
    sessionTransactions.push_back({username, gameName, amount});
}

//THIS PART NEEDS TO BE FIXED FOR PROPER TRANSACTIONS
bool TransactionLog::outputTransactions() const {
    // Append to daily transaction file
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