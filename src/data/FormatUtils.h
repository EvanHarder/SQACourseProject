#ifndef FORMAT_UTILS_H
#define FORMAT_UTILS_H

#include <string>

class FormatUtils {
public:
    // Pads text with added/removed width
    static std::string padText(const std::string& text, size_t width);

    // Converts a double into a string with padding (5000.0 -> "005000.00") 9 is default because i worked with account balance
    static std::string moneyToText(double amount, size_t totalWidth = 9);

    // Converts text into a double (used when taking transaction outputs)
    static double textToMoney(const std::string& moneyText);
};

#endif