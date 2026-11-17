// LogoutTransaction.h
//
// LogoutTransaction
// The logout command: ends the session. Records the end of session (00) line,
// has the session write all of this session's lines to the daily transaction
// file, and confirms. After logout only login is accepted (FrontEnd enforces
// this through requiresLogin).

#pragma once

#include "Transaction.h"

class LogoutTransaction : public Transaction {
public:
    // Returns the command word "logout".
    std::string getName() const override;

    // Writes the session's transactions to the daily transaction file and logs out.
    void execute(Session& session, Console& console) override;
};
