// TransactionLine.h
//
// TransactionLine
// Builds each kind of line in the daily transaction file, so every line
// layout is written in exactly one place. Fields are separated by one space;
// names are left-justified and space-filled, money is zero-filled (see Formatter).
//
//   01 create, 02 delete, 06 addcredit, 00 end of session (31 chars):
//     XX UUUUUUUUUUUUUUU TT CCCCCCCCC
//   05 refund (44 chars):
//     XX UUUUUUUUUUUUUUU SSSSSSSSSSSSSSS CCCCCCCCC
//   03 sell (52 chars):
//     XX IIIIIIIIIIIIIIIIIIIIIIIIII SSSSSSSSSSSSSSS PPPPPP
//   04 buy (68 chars):
//     XX IIIIIIIIIIIIIIIIIIIIIIIIII SSSSSSSSSSSSSSS UUUUUUUUUUUUUUU PPPPPP
//
// where U = username (buyer when there are two users), S = seller, T = user type,
// C = credit, I = game name, P = price.
//
// TransactionLine holds no data, so every method is static and it can't be created.

#pragma once

#include <string>

#include "Constants.h"

class TransactionLine {
public:
    TransactionLine() = delete;

    // Returns a create (01), delete (02) or add credit (06) line for one account.
    // code is one of Config::CODE_CREATE, CODE_DELETE or CODE_ADD_CREDIT.
    static std::string forAccount(const std::string& code, const std::string& username,
                                  const std::string& userType, Cents credit);

    // Returns the end of session (00) line, with the username and type blank and credit zero.
    static std::string forEndOfSession();

    // Returns a refund (05) line moving amount from the seller to the buyer.
    static std::string forRefund(const std::string& buyer, const std::string& seller, Cents amount);

    // Returns a sell (03) line for a game put up for sale.
    static std::string forSale(const std::string& gameName, const std::string& seller, Cents price);

    // Returns a buy (04) line for a game bought from the seller by the buyer.
    static std::string forPurchase(const std::string& gameName, const std::string& seller,
                                   const std::string& buyer, Cents price);
};
