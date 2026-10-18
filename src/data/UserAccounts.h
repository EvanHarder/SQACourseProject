/*
 * UserAccounts.h
 *
 * Class: UserAccounts
 * Intention: Loads the current user accounts file, finds users by name and
 * updates their credit. Each account is stored as a User record.
 */

#ifndef USER_ACCOUNTS_H
#define USER_ACCOUNTS_H

#include <string>
#include <vector>

// One account from the current user accounts file.
struct User {
    std::string username;       // up to 15 characters, may contain spaces
    double balance;             // available credit in dollars
    std::string permissions;    // user type code: AA, FS, BS or SS
};

class UserAccounts {
private:
    std::vector<User> users;    // every account loaded from the file
    std::string filename;       // path of the current user accounts file

public:
    // Stores the path of the accounts file; nothing is read yet.
    explicit UserAccounts(std::string accountFilename);

    // Reads every account from the file; returns false if the file cannot be opened.
    bool loadFromFile();

    // Writes every account back to the file in the fixed-width format; returns false if it cannot be written.
    bool saveToFile();

    // Returns the account with the given username, or nullptr if there is none.
    User* findUser(const std::string& username);

    // Sets a user's credit; returns false if the user does not exist.
    bool updateBalance(const std::string& username, double newBalance);

    // Returns every loaded account.
    const std::vector<User>& getUsers() const;
};

#endif