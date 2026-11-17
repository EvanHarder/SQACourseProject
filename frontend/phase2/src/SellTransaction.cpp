// SellTransaction.cpp
//
// Method definitions for SellTransaction. See SellTransaction.h for what the
// class is for.

#include "SellTransaction.h"

#include "Formatter.h"
#include "TransactionLine.h"

// Returns the command word "sell".
std::string SellTransaction::getName() const {
    return Config::COMMAND_SELL;
}

// Returns true if the user's type may sell games (everyone except buy-standard).
bool SellTransaction::isAllowedFor(const User& user) const {
    return user.canSell();
}

// Asks for a game name and price and lists the game, or prints why it can't.
void SellTransaction::execute(Session& session, Console& console) {
    std::string gameName = console.prompt("Enter game name:");
    std::string problem = findGameNameProblem(session, gameName);
    if (!problem.empty()) {
        console.printError(problem);
        return;
    }

    Cents price = 0;
    if (!readMoney(console, "Enter price:", "invalid price.", price)) {
        return;
    }
    // Free games (0.00) are allowed (client clarification)
    if (price > Config::MAX_PRICE) {
        console.printError("invalid price.");
        return;
    }

    std::string seller = session.getCurrentUser().getName();
    const bool listedThisSession = true;
    session.addGame(Game(gameName, seller, price, listedThisSession));
    session.recordTransaction(TransactionLine::forSale(gameName, seller, price));
    console.print("Game " + gameName + " listed for sale at " + Formatter::formatMoney(price) + ".");
}

// Returns the error message for an unusable game name, or "" if it is fine.
// A blank name or the END marker can't be stored in the files. Names only need
// to be unique among games for sale, including ones listed this session.
std::string SellTransaction::findGameNameProblem(Session& session, const std::string& gameName) const {
    if (gameName.empty() || gameName == Config::END_MARKER) {
        return "invalid game name.";
    }
    if (gameName.size() > Config::MAX_GAME_NAME_LENGTH) {
        return "game name too long.";
    }
    if (session.findGame(gameName) != nullptr) {
        return "game name already exists.";
    }
    return "";
}
