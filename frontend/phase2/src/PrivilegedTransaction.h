// PrivilegedTransaction.h
//
// PrivilegedTransaction
// Base class for privileged transactions: the ones only an admin (AA) may run
// (create, delete, refund and listaccounts). It overrides isAllowedFor once,
// so those commands inherit the admin check instead of each repeating it.
// FrontEnd runs the check before execute(), so a non-admin gets
// "permission denied." straight after typing the command.
//
// It is still abstract: subclasses provide getName() and execute().

#pragma once

#include "Transaction.h"

class PrivilegedTransaction : public Transaction {
public:
    // Returns true only if the given user is an admin.
    bool isAllowedFor(const User& user) const override;
};
