#include "UserAccounts.h"
#include "FormatUtils.h"
#include <fstream>

UserAccounts::UserAccounts(std::string accountFilename) : filename(std::move(accountFilename)) {}

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

User* UserAccounts::findUser(const std::string& username) {
    for (auto& user : users) {
        if (user.username == username) {
            return &user;
        }
    }
    return nullptr;
}

bool UserAccounts::updateBalance(const std::string& username, double newBalance) {
    User* user = findUser(username);
    if (user) {
        user->balance = newBalance;
        return true;
    }
    return false;
}

const std::vector<User>& UserAccounts::getUsers() const {
    return users;
}