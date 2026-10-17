#include "UserAccounts.h"
#include <fstream>
#include <sstream>

UserAccounts::UserAccounts(std::string accountFilename) : filename(std::move(accountFilename)) {}

bool UserAccounts::loadFromFile() {
    users.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string uname;
        double bal;
        if (iss >> uname >> bal) {
            users.push_back({uname, bal});
        }
    }
    return true;
}

bool UserAccounts::saveToFile() {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) return false;

    for (const auto& user : users) {
        file << user.username << " " << user.balance << "\n";
    }
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