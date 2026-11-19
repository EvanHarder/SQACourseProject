// PrivilegedTransaction.cpp
//
// Method definitions for PrivilegedTransaction. See PrivilegedTransaction.h
// for what the class is for.

#include "PrivilegedTransaction.h"

// Returns true only if the given user has admin privileges (admin or account manager).
bool PrivilegedTransaction::isAllowedFor(const User& user) const {
    return user.hasAdminPrivileges();
}
