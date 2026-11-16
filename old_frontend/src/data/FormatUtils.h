/*
 * FormatUtils.h
 *
 * Class: FormatUtils
 * Intention: Helper functions for the fixed-width file formats: padding and
 * trimming text fields, and converting money between numbers and text.
 */

#ifndef FORMAT_UTILS_H
#define FORMAT_UTILS_H

#include <string>

class FormatUtils {
public:
    // Pads text with spaces on the right to exactly width characters, or cuts it down to width.
    static std::string padText(const std::string& text, size_t width);

    // Removes leading and trailing spaces, tabs and line endings (used when reading fixed-width fields).
    static std::string trimText(const std::string& text);

    // Converts an amount to text with two decimals and leading zeros, e.g. 5000.0 -> "005000.00" for width 9.
    static std::string moneyToText(double amount, size_t totalWidth = 9);

    // Converts money text such as "005000.00" into a number; returns 0.0 if the text is not a number.
    static double textToMoney(const std::string& moneyText);
};

#endif