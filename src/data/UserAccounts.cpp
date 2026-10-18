/*
 * UserAccounts.cpp
 *
 * Implementation of the UserAccounts class: loads, searches and saves the
 * current user accounts file. See UserAccounts.h for each method.
 *
 * File format (fixed width, one account per line, ends with an END line):
 *     username (16) | type (2) | space | credit (9)
 *     admin01         AA 005000.00
 */

#include "UserAccounts.h"
#include "FormatUtils.h"
#include <fstream>

// Stores the path; nothing is read until loadFromFile().
UserAccounts::UserAccounts(std::string accountFilename) : filename(std::move(accountFilename)) {}

// Reads every account line up to END; usernames may contain spaces (e.g. "Jane Doe").
bool UserAccounts::loadFromFile() {
    users.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (FormatUtils::trimText(line) == "END") break;
        if (line.length() < 28) continue;

        std::string uname = FormatUtils::trimText(line.substr(0, 16));
        std::string type = line.substr(16, 2);
        double bal = FormatUtils::textToMoney(line.substr(19, 9));
        users.push_back({uname, bal, type});
    }
    return true;
}

// Writes every account in the fixed-width format, followed by the END line.
bool UserAccounts::saveToFile() {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) return false;

    for (const auto& user : users) {
        std::string pUser = FormatUtils::padText(user.username, 16);
        std::string pPermissions = FormatUtils::padText(user.permissions, 3);
        std::string pBalance = FormatUtils::moneyToText(user.balance, 9);
        file << pUser << pPermissions << pBalance << "\n";
    }
    file << FormatUtils::padText("END", 28) << "\n";

    return true;
}

// Returns the account with this exact username, or nullptr.
User* UserAccounts::findUser(const std::string& username) {
    for (auto& user : users) {
        if (user.username == username) {
            return &user;
        }
    }
    return nullptr;
}

// Sets a user's credit; returns false if the user does not exist.
bool UserAccounts::updateBalance(const std::string& username, double newBalance) {
    User* user = findUser(username);
    if (user) {
        user->balance = newBalance;
        return true;
    }
    return false;
}

// Returns every loaded account (used by listaccounts).
const std::vector<User>& UserAccounts::getUsers() const {
    return users;
}