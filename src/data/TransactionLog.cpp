/*
 * TransactionLog.cpp
 *
 * Implementation of the TransactionLog class: stores successful transactions
 * and writes them to the daily transaction file.
 * See TransactionLog.h for the line formats and the description of each method.
 */

#include "TransactionLog.h"
#include "FormatUtils.h"

#include <fstream>
#include <utility>

namespace {
    const std::size_t USERNAME_WIDTH = 15;   // usernames are padded to 15 characters
    const std::size_t GAME_NAME_WIDTH = 25;  // game names are padded to 25 characters

    // Builds a 00, 01, 02 or 06 line: code, username, user type and credit (e.g. 005000.00).
    std::string accountRecord(const std::string& code, const std::string& username,
                              const std::string& userType, double credit) {
        return code + " " + FormatUtils::padText(username, USERNAME_WIDTH) + " " +
               userType + " " + FormatUtils::moneyToText(credit, 9);
    }
}

// Stores the path; nothing is written until outputTransactions().
TransactionLog::TransactionLog(std::string logFilename)
    : dailyLogFilename(std::move(logFilename)) {}

// 01: new user, type, starting credit.
void TransactionLog::recordCreate(const std::string& username, const std::string& userType,
                                  double credit) {
    sessionTransactions.push_back(accountRecord("01", username, userType, credit));
}

// 02: deleted user, type, credit at deletion.
void TransactionLog::recordDelete(const std::string& username, const std::string& userType,
                                  double credit) {
    sessionTransactions.push_back(accountRecord("02", username, userType, credit));
}

// 03: game (25), seller (15), price (e.g. 015.00).
void TransactionLog::recordSell(const std::string& gameName, const std::string& seller,
                                double price) {
    sessionTransactions.push_back(
        "03 " + FormatUtils::padText(gameName, GAME_NAME_WIDTH) + " " +
        FormatUtils::padText(seller, USERNAME_WIDTH) + " " +
        FormatUtils::moneyToText(price, 6));
}

// 04: game (25), seller (15), buyer (15), price.
void TransactionLog::recordBuy(const std::string& gameName, const std::string& seller,
                               const std::string& buyer, double price) {
    sessionTransactions.push_back(
        "04 " + FormatUtils::padText(gameName, GAME_NAME_WIDTH) + " " +
        FormatUtils::padText(seller, USERNAME_WIDTH) + " " +
        FormatUtils::padText(buyer, USERNAME_WIDTH) + " " +
        FormatUtils::moneyToText(price, 6));
}

// 05: buyer (15), seller (15), amount refunded (e.g. 000010.00).
void TransactionLog::recordRefund(const std::string& buyer, const std::string& seller,
                                  double credit) {
    sessionTransactions.push_back(
        "05 " + FormatUtils::padText(buyer, USERNAME_WIDTH) + " " +
        FormatUtils::padText(seller, USERNAME_WIDTH) + " " +
        FormatUtils::moneyToText(credit, 9));
}

// 06: account, type, amount added.
void TransactionLog::recordAddCredit(const std::string& username, const std::string& userType,
                                     double credit) {
    sessionTransactions.push_back(accountRecord("06", username, userType, credit));
}

// 00: logged-in user, type, credit at logout.
void TransactionLog::recordEndOfSession(const std::string& username,
                                        const std::string& userType, double credit) {
    sessionTransactions.push_back(accountRecord("00", username, userType, credit));
}

// Appends only the lines not yet written, so each logout adds its own session to the file.
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
