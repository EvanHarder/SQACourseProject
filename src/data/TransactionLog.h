//Used for output, also makes sure the transaction codes are formatted
#ifndef TRANSACTION_LOG_H
#define TRANSACTION_LOG_H

#include <string>
#include <vector>

struct Transaction {
    std::string username;
    std::string gameName;
    double amount;
};

class TransactionLog {
private:
    std::vector<Transaction> sessionTransactions;
    std::string dailyLogFilename;

public:
    explicit TransactionLog(std::string logFilename);
    
    //Adds a transaction to a list
    void recordTransaction(const std::string& username, const std::string& gameName, double amount);
    //Format and output the list to file function
    bool outputTransactions() const;
};

#endif