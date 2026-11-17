// TransactionLine.cpp
//
// Method definitions for TransactionLine. See TransactionLine.h for the line layouts.

#include "TransactionLine.h"

#include "Formatter.h"

// Returns a create (01), delete (02) or add credit (06) line for one account.
std::string TransactionLine::forAccount(const std::string& code, const std::string& username,
                                        const std::string& userType, Cents credit) {
    return code + " "
        + Formatter::padRight(username, Config::USERNAME_WIDTH) + " "
        + Formatter::padRight(userType, Config::USER_TYPE_WIDTH) + " "
        + Formatter::formatMoney(credit, Config::CREDIT_WIDTH);
}

// Returns the end of session (00) line: same layout as an account line, with unused fields empty.
std::string TransactionLine::forEndOfSession() {
    return forAccount(Config::CODE_END_OF_SESSION, "", "", 0);
}

// Returns a refund (05) line moving amount from the seller to the buyer.
std::string TransactionLine::forRefund(const std::string& buyer, const std::string& seller, Cents amount) {
    return Config::CODE_REFUND + " "
        + Formatter::padRight(buyer, Config::USERNAME_WIDTH) + " "
        + Formatter::padRight(seller, Config::USERNAME_WIDTH) + " "
        + Formatter::formatMoney(amount, Config::CREDIT_WIDTH);
}

// Returns a sell (03) line for a game put up for sale.
std::string TransactionLine::forSale(const std::string& gameName, const std::string& seller, Cents price) {
    return Config::CODE_SELL + " "
        + Formatter::padRight(gameName, Config::GAME_NAME_WIDTH) + " "
        + Formatter::padRight(seller, Config::USERNAME_WIDTH) + " "
        + Formatter::formatMoney(price, Config::PRICE_WIDTH);
}

// Returns a buy (04) line for a game bought from the seller by the buyer.
std::string TransactionLine::forPurchase(const std::string& gameName, const std::string& seller,
                                         const std::string& buyer, Cents price) {
    return Config::CODE_BUY + " "
        + Formatter::padRight(gameName, Config::GAME_NAME_WIDTH) + " "
        + Formatter::padRight(seller, Config::USERNAME_WIDTH) + " "
        + Formatter::padRight(buyer, Config::USERNAME_WIDTH) + " "
        + Formatter::formatMoney(price, Config::PRICE_WIDTH);
}
