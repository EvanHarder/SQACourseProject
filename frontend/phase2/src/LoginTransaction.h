// LoginTransaction.h
//
// LoginTransaction
// The login command: starts a session. Refuses if someone is already logged
// in, otherwise asks for a username, has the session read the input files and
// log that user in, and welcomes them with their user type. It writes nothing
// to the daily transaction file.
//
// It is the only transaction that can run while nobody is logged in.

#pragma once

#include "Transaction.h"

class LoginTransaction : public Transaction {
public:
    // Returns the command word "login".
    std::string getName() const override;

    // Returns false: login is the one command allowed before logging in.
    bool requiresLogin() const override;

    // Logs in the user whose username is entered, or prints why it can't.
    void execute(Session& session, Console& console) override;
};
