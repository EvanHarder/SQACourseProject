// Makes a user sturcutre, then makes you able to search and update balances, also reads from the user file.
// Should have remove function?
#ifndef USER_ACCOUNTS_H
#define USER_ACCOUNTS_H

#include <string>
#include <vector>

struct User {
    std::string username;
    double balance;
    // std::string password;
    std::string permissions;

};

class UserAccounts {
private:
    std::vector<User> users;
    std::string filename;

public:
    explicit UserAccounts(std::string accountFilename);
    
    bool loadFromFile();
    bool saveToFile();
    
    User* findUser(const std::string& username);
    bool updateBalance(const std::string& username, double newBalance);
    
    const std::vector<User>& getUsers() const;
};

#endif