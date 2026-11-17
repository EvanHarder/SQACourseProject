// Constants.h
//
// Every fixed value the Front End uses, kept in one place: file names, field
// widths, limits, user type codes, transaction codes and command words.
// Values come from the project requirements and the client's clarifications.
// If the client changes a limit or a field width, this is the only file that
// needs to change.
//
// Money is stored in whole cents (see Cents below), so every money limit here
// is written in cents with the dollar amount in the comment.

#pragma once

#include <cstddef>
#include <string>

// An amount of money in whole cents ($15.50 is stored as 1550).
// Integers are exact, so comparisons against limits like $1000.00 never suffer
// from floating-point rounding the way a double would.
using Cents = long long;

namespace Config {

    // ---- Input and output files ----
    // The four file names are given on the command line (see main.cpp).

    const std::string END_MARKER = "END";  // name on the last line of every input file

    // ---- Field widths in the fixed-width files ----
    // Line lengths and column positions are worked out from these, not stored separately.

    const std::size_t TRANSACTION_CODE_WIDTH = 2;   // "00" to "06"
    const std::size_t USERNAME_WIDTH         = 15;  // left-justified, padded with spaces
    const std::size_t USER_TYPE_WIDTH        = 2;   // "AA", "FS", "BS" or "SS"
    const std::size_t GAME_NAME_WIDTH        = 26;  // left-justified, padded with spaces
    const std::size_t CREDIT_WIDTH           = 9;   // e.g. 001000.00
    const std::size_t PRICE_WIDTH            = 6;   // e.g. 015.00

    // ---- Limits ----

    const std::size_t MAX_USERNAME_LENGTH  = 15;  // longest allowed username
    const std::size_t MAX_GAME_NAME_LENGTH = 26;  // spec says 25, but Phase 1 tests accept 26 (client: example width wins)

    const Cents MAX_PRICE              = 99999;     // $999.99, highest price a game can be listed at
    const Cents MAX_CREDIT             = 99999999;  // $999,999.99, highest balance any account can hold
    const Cents MAX_CREDIT_PER_SESSION = 100000;    // $1000.00 that can be added to one account per session (admins included)

    // ---- User type codes ----

    const std::string USER_TYPE_ADMIN         = "AA";  // can run every transaction
    const std::string USER_TYPE_FULL_STANDARD = "FS";  // can buy and sell
    const std::string USER_TYPE_BUY_STANDARD  = "BS";  // can buy, cannot sell
    const std::string USER_TYPE_SELL_STANDARD = "SS";  // can sell, cannot buy

    // ---- Daily transaction file codes ----

    const std::string CODE_END_OF_SESSION = "00";  // written at every logout
    const std::string CODE_CREATE         = "01";
    const std::string CODE_DELETE         = "02";
    const std::string CODE_SELL           = "03";
    const std::string CODE_BUY            = "04";
    const std::string CODE_REFUND         = "05";
    const std::string CODE_ADD_CREDIT     = "06";

    // ---- Command words (what the user types) ----

    const std::string COMMAND_LOGIN         = "login";
    const std::string COMMAND_LOGOUT        = "logout";
    const std::string COMMAND_CREATE        = "create";
    const std::string COMMAND_DELETE        = "delete";
    const std::string COMMAND_SELL          = "sell";
    const std::string COMMAND_BUY           = "buy";
    const std::string COMMAND_REFUND        = "refund";
    const std::string COMMAND_ADD_CREDIT    = "addcredit";
    const std::string COMMAND_LIST_GAMES    = "list";
    const std::string COMMAND_LIST_ACCOUNTS = "listaccounts";  // placeholder name until the client confirms one
    const std::string COMMAND_SEARCH        = "search";        // added by the client in Phase 3
    
}