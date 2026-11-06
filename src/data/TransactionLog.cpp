/*
 * TransactionLog.cpp
 *
 * Implementation of the TransactionLog class: stores successful transactions
 * and writes them to the daily transaction file.
 * See TransactionLog.h for the description of each method.
 */

#include "TransactionLog.h"
#include "FormatUtils.h"

#include <fstream>
#include <utility>

namespace {
    const std::size_t USERNAME_WIDTH = 15;
    const std::size_t GAME_NAME_WIDTH = 25;

    std::string accountRecord(const std::string& code, const std::string& username,
                              const std::string& userType, double credit) {
        return code + " " + FormatUtils::padText(username, USERNAME_WIDTH) + " " +
               userType + " " + FormatUtils::moneyToText(credit, 9);
    }
}

TransactionLog::TransactionLog(std::string logFilename)
    : dailyLogFilename(std::move(logFilename)) {}

void TransactionLog::recordCreate(const std::string& username, const std::string& userType,
                                  double credit) {
    sessionTransactions.push_back(accountRecord("01", username, userType, credit));
}

void TransactionLog::recordDelete(const std::string& username, const std::string& userType,
                                  double credit) {
    sessionTransactions.push_back(accountRecord("02", username, userType, credit));
}

void TransactionLog::recordSell(const std::string& gameName, const std::string& seller,
                                double price) {
    sessionTransactions.push_back(
        "03 " + FormatUtils::padText(gameName, GAME_NAME_WIDTH) + " " +
        FormatUtils::padText(seller, USERNAME_WIDTH) + " " +
        FormatUtils::moneyToText(price, 6));
}

void TransactionLog::recordBuy(const std::string& gameName, const std::string& seller,
                               const std::string& buyer, double price) {
    sessionTransactions.push_back(
        "04 " + FormatUtils::padText(gameName, GAME_NAME_WIDTH) + " " +
        FormatUtils::padText(seller, USERNAME_WIDTH) + " " +
        FormatUtils::padText(buyer, USERNAME_WIDTH) + " " +
        FormatUtils::moneyToText(price, 6));
}

void TransactionLog::recordRefund(const std::string& buyer, const std::string& seller,
                                  double credit) {
    sessionTransactions.push_back(
        "05 " + FormatUtils::padText(buyer, USERNAME_WIDTH) + " " +
        FormatUtils::padText(seller, USERNAME_WIDTH) + " " +
        FormatUtils::moneyToText(credit, 9));
}

void TransactionLog::recordAddCredit(const std::string& username, const std::string& userType,
                                     double credit) {
    sessionTransactions.push_back(accountRecord("06", username, userType, credit));
}

void TransactionLog::recordEndOfSession(const std::string& username,
                                        const std::string& userType, double credit) {
    sessionTransactions.push_back(accountRecord("00", username, userType, credit));
}

bool TransactionLog::outputTransactions() const {
    std::ofstream file(dailyLogFilename, std::ios::app);
    if (!file.is_open()) {
        return false;
    }

    for (; writtenTransactions < sessionTransactions.size(); ++writtenTransactions) {
        file << sessionTransactions[writtenTransactions] << '\n';
        if (!file) {
            return false;
        }
    }

    file.flush();
    return file.good();
}
