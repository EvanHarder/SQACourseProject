// BuyTransaction.cpp
//
// Method definitions for BuyTransaction. See BuyTransaction.h for what the
// class is for and the order of its checks.

#include "BuyTransaction.h"

#include "Formatter.h"
#include "TransactionLine.h"

// Returns the command word "buy".
std::string BuyTransaction::getName() const {
    return Config::COMMAND_BUY;
}

// Returns true if the user's type may buy games (everyone except sell-standard).
bool BuyTransaction::isAllowedFor(const User& user) const {
    return user.canBuy();
}

// Asks for a game and its seller and buys it, or prints why it can't.
// The game, buyer and seller pointers are safe to use throughout: nothing is
// added to or removed from the session's users or games during a purchase.
void BuyTransaction::execute(Session& session, Console& console) {
    std::string gameName = console.prompt("Enter game name:");
    Game* game = session.findGame(gameName);
    if (game == nullptr) {
        console.printError("game does not exist.");
        return;
    }
    // Spec: a game put up for sale can't be traded until the next session
    if (game->isListedThisSession()) {
        console.printError("game not available until next session.");
        return;
    }

    User* seller = readExistingUser(session, console, "Enter seller username:");
    if (seller == nullptr) {
        return;
    }
    if (game->getSeller() != seller->getName()) {
        console.printError("seller does not sell that game.");
        return;
    }

    User& buyer = session.getCurrentUser();
    if (buyer.getName() == seller->getName()) {
        console.printError("cannot buy your own game.");
        return;
    }
    if (session.ownsGame(buyer.getName(), gameName)) {
        console.printError("game already owned.");
        return;
    }

    Cents price = game->getPrice();
    if (buyer.getCredit() < price) {
        console.printError("insufficient credit.");
        return;
    }
    if (seller->getCredit() + price > Config::MAX_CREDIT) {
        console.printError("credit limit exceeded.");
        return;
    }

    buyer.removeCredit(price);
    seller->addCredit(price);
    session.addToCollection(buyer.getName(), gameName);
    session.recordTransaction(TransactionLine::forPurchase(gameName, seller->getName(), buyer.getName(), price));
    console.print("Purchase successful. " + gameName + " bought from " + seller->getName()
                  + " for " + Formatter::formatMoney(price) + ".");
}
