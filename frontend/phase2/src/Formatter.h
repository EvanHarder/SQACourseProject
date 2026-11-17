// Formatter.h
//
// Formatter
// Text and money helpers shared by everything that reads or writes the
// fixed-width files (FileManager, TransactionLine) and by transactions that
// read amounts from the user. Keeping them here means padding and money rules
// are written once.
//
// Money is always in whole cents (see Cents in Constants.h). Formatter only
// checks that text is a well-formed amount; limits such as the maximum price
// are checked by the transactions, which print their own error messages.
//
// Formatter holds no data, so every method is static and it can't be created.

#pragma once

#include <cstddef>
#include <string>

#include "Constants.h"

class Formatter {
public:
    Formatter() = delete;

    // Returns text left-justified and filled with spaces to the given width.
    // Text already at or over the width is returned unchanged; callers validate lengths first.
    static std::string padRight(const std::string& text, std::size_t width);

    // Returns text with trailing spaces (and a trailing '\r' from Windows line endings) removed.
    static std::string trimRight(const std::string& text);

    // Returns the field of the given width starting at the given column, with its padding removed.
    // Returns whatever part exists if the line is too short, or "" if it starts past the end.
    static std::string extractField(const std::string& line, std::size_t start, std::size_t width);

    // Returns cents as dollars with two decimals, e.g. 1500 -> "15.00".
    // If width is given, zeros are added on the left to fill it, e.g. 1500, 6 -> "015.00".
    static std::string formatMoney(Cents cents, std::size_t width = 0);

    // Reads a dollar amount like "15", "15.5" or "015.00" into cents.
    // Returns false (and leaves cents unchanged) if the text isn't a valid non-negative amount
    // with at most two decimal places.
    static bool parseMoney(const std::string& text, Cents& cents);

private:
    // Returns true if text is non-empty and every character is a digit 0-9.
    static bool isAllDigits(const std::string& text);
};
