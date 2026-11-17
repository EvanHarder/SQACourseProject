// BuyTransaction.h
//
// BuyTransaction
// The buy command: the logged-in user buys a copy of a game for sale.
// Allowed for every user type except sell-standard (SS). Asks for the game
// name and checks it, then asks for the seller's username and checks the
// purchase. On success the price moves from the buyer's credit to the
// seller's, the game is added to the buyer's collection (it stays for sale
// for others), and a buy (04) line is recorded.
//
// Checks, in order (each stops the purchase with its own error):
//   after the game name:   game exists, game wasn't listed this session
//   after the seller name: seller exists, seller sells this game,
//                          buyer isn't the seller, buyer doesn't own it yet,
//                          buyer has enough credit, seller stays under the credit cap

#pragma once

#include <string>

#include "Transaction.h"

class BuyTransaction : public Transaction {
public:
    // Returns the command word "buy".
    std::string getName() const override;

    // Returns true if the user's type may buy games (everyone except sell-standard).
    bool isAllowedFor(const User& user) const override;

    // Asks for a game and its seller and buys it, or prints why it can't.
    void execute(Session& session, Console& console) override;
};
