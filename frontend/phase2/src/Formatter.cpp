// Formatter.cpp
//
// Method definitions for Formatter. See Formatter.h for what the class is for.

#include "Formatter.h"

// Most digits accepted before the decimal point. Far above any real limit, but
// small enough that converting to cents can never overflow a long long.
static const std::size_t MAX_DOLLAR_DIGITS = 12;

// Returns text left-justified and filled with spaces to the given width.
std::string Formatter::padRight(const std::string& text, std::size_t width) {
    if (text.size() >= width) {
        return text;
    }
    return text + std::string(width - text.size(), ' ');
}

// Returns text with trailing spaces and carriage returns removed.
std::string Formatter::trimRight(const std::string& text) {
    std::size_t end = text.find_last_not_of(" \r");
    if (end == std::string::npos) {
        return "";
    }
    return text.substr(0, end + 1);
}

// Returns the field of the given width starting at the given column, with its padding removed.
std::string Formatter::extractField(const std::string& line, std::size_t start, std::size_t width) {
    if (start >= line.size()) {
        return "";
    }
    return trimRight(line.substr(start, width));
}

// Returns cents as dollars with two decimals, zero-filled on the left to width if given.
std::string Formatter::formatMoney(Cents cents, std::size_t width) {
    std::string dollars = std::to_string(cents / 100);
    std::string centsPart = std::to_string(cents % 100);
    if (centsPart.size() < 2) {
        centsPart = "0" + centsPart;
    }

    std::string money = dollars + "." + centsPart;
    if (money.size() < width) {
        money = std::string(width - money.size(), '0') + money;
    }
    return money;
}

// Reads a dollar amount into cents without going through a double.
// Accepts "D", "D.d" or "D.dd" where D is one or more digits.
bool Formatter::parseMoney(const std::string& text, Cents& cents) {
    std::size_t dot = text.find('.');
    std::string dollarPart = text.substr(0, dot);
    std::string centsPart = (dot == std::string::npos) ? "" : text.substr(dot + 1);

    if (!isAllDigits(dollarPart) || dollarPart.size() > MAX_DOLLAR_DIGITS) {
        return false;
    }
    if (dot != std::string::npos && (!isAllDigits(centsPart) || centsPart.size() > 2)) {
        return false;
    }

    // "5" after the dot means 50 cents, so pad the cents part to two digits
    while (centsPart.size() < 2) {
        centsPart += "0";
    }

    cents = std::stoll(dollarPart) * 100 + std::stoll(centsPart);
    return true;
}

// Returns true if text is non-empty and every character is a digit 0-9.
bool Formatter::isAllDigits(const std::string& text) {
    if (text.empty()) {
        return false;
    }
    for (char c : text) {
        if (c < '0' || c > '9') {
            return false;
        }
    }
    return true;
}
