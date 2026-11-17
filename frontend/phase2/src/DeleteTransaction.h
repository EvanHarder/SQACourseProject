// DeleteTransaction.h
//
// DeleteTransaction
// The delete command (admin only): removes a user account and cancels every
// game that user has for sale. Asks for the username, which must be an
// existing user other than the one logged in. Records a delete (02) line with
// the deleted user's type and current credit. After this, no transaction in
// the session can use the user or their games.

#pragma once

#include <string>

#include "PrivilegedTransaction.h"

class DeleteTransaction : public PrivilegedTransaction {
public:
    // Returns the command word "delete".
    std::string getName() const override;

    // Asks for a username and deletes that account, or prints why it can't.
    void execute(Session& session, Console& console) override;
};
