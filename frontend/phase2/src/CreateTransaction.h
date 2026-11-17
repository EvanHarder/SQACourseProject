// CreateTransaction.h
//
// CreateTransaction
// The create command (admin only): adds a new user account. Asks for the new
// username and checks it straight away, then asks for the user type (AA, FS,
// BS or SS). A new account starts with 0.00 credit. Records a create (01)
// line; the account file itself is only updated by the Back End, but the new
// user can be used by later transactions in this session.

#pragma once

#include <string>

#include "PrivilegedTransaction.h"

class CreateTransaction : public PrivilegedTransaction {
public:
    // Returns the command word "create".
    std::string getName() const override;

    // Asks for a username and user type and creates the account, or prints why it can't.
    void execute(Session& session, Console& console) override;

private:
    // Returns the error message for an unusable new username, or "" if it is fine.
    std::string findUsernameProblem(Session& session, const std::string& username) const;
};
