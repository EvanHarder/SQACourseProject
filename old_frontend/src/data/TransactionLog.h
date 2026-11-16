/*
 * TransactionLog.h
 *
 * Class: TransactionLog
 * Intention: Collects the successful transactions of a session as fixed-width
 * lines and writes them to the daily transaction file at logout, for the Back
 * End to process.
 *
 * Line formats (_ is a space):
 *   00, 01, 02, 06:  XX_UUUUUUUUUUUUUUU_TT_CCCCCCCCC                       (user, type, credit)
 *   03 sell:         XX_IIIIIIIIIIIIIIIIIIIIIIIII_SSSSSSSSSSSSSSS_PPPPPP   (game, seller, price)
 *   04 buy:          XX_IIIIIIIIIIIIIIIIIIIIIIIII_SSSSSSSSSSSSSSS_UUUUUUUUUUUUUUU_PPPPPP
 *                                                                          (game, seller, buyer, price)
 *   05 refund:       XX_UUUUUUUUUUUUUUU_SSSSSSSSSSSSSSS_CCCCCCCCC          (buyer, seller, credit)
 */

#ifndef TRANSACTION_LOG_H
#define TRANSACTION_LOG_H

#include <cstddef>
#include <string>
#include <vector>

class TransactionLog {
private:
    std::vector<std::string> sessionTransactions;   // every line recorded so far, in order
    std::string dailyLogFilename;                   // path of the daily transaction file
    mutable std::size_t writtenTransactions = 0;    // how many lines have already been written to the file

public:
    // Stores the path of the daily transaction file; nothing is written yet.
    explicit TransactionLog(std::string logFilename);

    // Records a create (01): the new user, their type and starting credit.
    void recordCreate(const std::string& username, const std::string& userType, double credit);

    // Records a delete (02): the deleted user, their type and credit.
    void recordDelete(const std::string& username, const std::string& userType, double credit);

    // Records a sell (03): the game, its seller and its price.
    void recordSell(const std::string& gameName, const std::string& seller, double price);

    // Records a buy (04): the game, its seller, the buyer and the price paid.
    void recordBuy(const std::string& gameName, const std::string& seller,
                   const std::string& buyer, double price);

    // Records a refund (05): the buyer, the seller and the amount moved.
    void recordRefund(const std::string& buyer, const std::string& seller, double credit);

    // Records an addcredit (06): the account, its type and the amount added.
    void recordAddCredit(const std::string& username, const std::string& userType, double credit);

    // Records the end of session (00): the logged-in user, their type and credit at logout.
    void recordEndOfSession(const std::string& username, const std::string& userType, double credit);

    // Appends the lines recorded since the last call to the daily transaction file;
    // returns false if the file cannot be opened or written.
    bool outputTransactions() const;
};

#endif
