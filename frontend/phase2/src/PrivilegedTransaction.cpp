// PrivilegedTransaction.cpp
//
// Method definitions for PrivilegedTransaction. See PrivilegedTransaction.h
// for what the class is for.

#include "PrivilegedTransaction.h"

// Returns true only if the given user is an admin.
bool PrivilegedTransaction::isAllowedFor(const User& user) const {
    return user.isAdmin();
}
