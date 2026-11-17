// AddCreditTransaction.h
//
// AddCreditTransaction
// The addcredit command: adds credit to an account. Any logged-in user may
// run it. A standard user is asked only for the amount and it goes to their
// own account; an admin is asked for the amount and then the username of the
// account to add to. Records an add credit (06) line holding the amount added.
//
// Checks, in order: amount is valid and above zero, (admin) the account
// exists, the account stays within $1000.00 added this session, and the
// account stays under the credit cap.

#pragma once

#include <string>

#include "Transaction.h"

class AddCreditTransaction : public Transaction {
public:
    // Returns the command word "addcredit".
    std::string getName() const override;

    // Asks for an amount (and, for an admin, a username) and adds the credit,
    // or prints why it can't.
    void execute(Session& session, Console& console) override;
};
