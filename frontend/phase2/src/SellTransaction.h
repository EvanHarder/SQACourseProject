// SellTransaction.h
//
// SellTransaction
// The sell command: puts a game up for sale under the logged-in user's name.
// Allowed for every user type except buy-standard (BS). Asks for the game name
// and checks it straight away, then asks for the price (0.00 to 999.99).
// Records a sell (03) line. The new game is marked as listed this session, so
// it can't be bought until the next session.

#pragma once

#include <string>

#include "Transaction.h"

class SellTransaction : public Transaction {
public:
    // Returns the command word "sell".
    std::string getName() const override;

    // Returns true if the user's type may sell games (everyone except buy-standard).
    bool isAllowedFor(const User& user) const override;

    // Asks for a game name and price and lists the game, or prints why it can't.
    void execute(Session& session, Console& console) override;

private:
    // Returns the error message for an unusable game name, or "" if it is fine.
    std::string findGameNameProblem(Session& session, const std::string& gameName) const;
};
