/*
 * FormatUtils.cpp
 *
 * Implementation of the FormatUtils helpers for the fixed-width file formats.
 * See FormatUtils.h for the description of each function.
 */

#include "FormatUtils.h"
#include <iomanip>
#include <iostream>
#include <sstream>

// Cuts text longer than width, otherwise adds spaces on the right.
std::string FormatUtils::padText(const std::string& text, size_t width) {
    if (text.length() >= width) {
        return text.substr(0, width);
    }
    return text + std::string(width - text.length(), ' ');
}

// Strips spaces, tabs and line endings from both ends; returns "" for a blank field.
std::string FormatUtils::trimText(const std::string& text) {
    const char* whitespace = " \t\r\n";
    size_t first = text.find_first_not_of(whitespace);
    if (first == std::string::npos) {
        return "";
    }
    size_t last = text.find_last_not_of(whitespace);
    return text.substr(first, last - first + 1);
}

// Formats the amount with two decimals, then adds leading zeros up to totalWidth.
std::string FormatUtils::moneyToText(double amount, size_t totalWidth) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << amount;
    std::string formatted = oss.str();

    if (formatted.length() < totalWidth) {
        formatted = std::string(totalWidth - formatted.length(), '0') + formatted;
    }
    return formatted;
}

// Parses the number; prints a warning and returns 0.0 if the text is not a number.
double FormatUtils::textToMoney(const std::string& moneyText) {
    try {
        return std::stod(moneyText);
    } catch (...) {
        std::cerr << "Failed to convert text to money: '" << moneyText << "\n";
        return 0.0;
    }
}