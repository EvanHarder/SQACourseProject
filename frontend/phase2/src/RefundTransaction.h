// RefundTransaction.h
//
// RefundTransaction
// The refund command (admin only): moves credit from a seller's account back
// to a buyer's account. Asks for the buyer's username, the seller's username
// and the amount, checking each answer straight away. Records a refund (05)
// line.
//
// Checks, in order: buyer exists, seller exists, buyer and seller differ,
// amount is valid and above zero, seller has enough credit, buyer stays under
// the credit cap.

#pragma once

#include <string>

#include "PrivilegedTransaction.h"

class RefundTransaction : public PrivilegedTransaction {
public:
    // Returns the command word "refund".
    std::string getName() const override;

    // Asks for a buyer, seller and amount and moves the credit, or prints why it can't.
    void execute(Session& session, Console& console) override;
};
