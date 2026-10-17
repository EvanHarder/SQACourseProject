#include "FormatUtils.h"
#include <iomanip>
#include <iostream>
#include <sstream>

std::string FormatUtils::padText(const std::string& text, size_t width) {
    //Trims down text
    if (text.length() >= width) {
        return text.substr(0, width);
    }
    //Text added with padding
        return text + std::string(width - text.length(), ' ');
}

std::string FormatUtils::moneyToText(double amount, size_t totalWidth) {
    //Formatting number (01000 = 1000.00)
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << amount;
    std::string formatted = oss.str();
    
    // Pad with leading zeros if necessary
    if (formatted.length() < totalWidth) {
        formatted = std::string(totalWidth - formatted.length(), '0') + formatted;
    }
    return formatted;
}

double FormatUtils::textToMoney(const std::string& moneyText) {
    try {
        return std::stod(moneyText);
    } catch (...) {
        std::cerr << "Failed to convert text to money: '" << moneyText << "\n";
        return 0.0;
    }
}