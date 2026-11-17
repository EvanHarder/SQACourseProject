// RefundTransaction.cpp
//
// Method definitions for RefundTransaction. See RefundTransaction.h for what
// the class is for and the order of its checks.

#include "RefundTransaction.h"

#include "Formatter.h"
#include "TransactionLine.h"

// Returns the command word "refund".
std::string RefundTransaction::getName() const {
    return Config::COMMAND_REFUND;
}

// Asks for a buyer, seller and amount and moves the credit, or prints why it can't.
// The buyer and seller pointers stay valid: no users are added or removed here.
void RefundTransaction::execute(Session& session, Console& console) {
    User* buyer = readExistingUser(session, console, "Enter buyer username:");
    if (buyer == nullptr) {
        return;
    }
    User* seller = readExistingUser(session, console, "Enter seller username:");
    if (seller == nullptr) {
        return;
    }
    if (buyer->getName() == seller->getName()) {
        console.printError("buyer and seller must be different.");
        return;
    }

    Cents amount = 0;
    if (!readMoney(console, "Enter amount:", "invalid amount.", amount)) {
        return;
    }
    if (amount == 0) {
        console.printError("invalid amount.");
        return;
    }
    // The refunded credit comes out of the seller's account (client clarification)
    if (seller->getCredit() < amount) {
        console.printError("insufficient credit.");
        return;
    }
    if (buyer->getCredit() + amount > Config::MAX_CREDIT) {
        console.printError("credit limit exceeded.");
        return;
    }

    seller->removeCredit(amount);
    buyer->addCredit(amount);
    session.recordTransaction(TransactionLine::forRefund(buyer->getName(), seller->getName(), amount));
    console.print("Refund of " + Formatter::formatMoney(amount) + " from " + seller->getName()
                  + " to " + buyer->getName() + " complete.");
}
