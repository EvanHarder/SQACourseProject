/*
 * ResultPrinter.cpp
 *
 * Implementation of the ResultPrinter class: prints one result block per
 * transaction. See ResultPrinter.h for the description of each method.
 */

#include "io/ResultPrinter.h"

namespace {
    const char* SUCCESS_TEXT = "SUCCESS";
    const char* FAILURE_TEXT = "FAILURE";
    const char* YES_TEXT = "Yes";
    const char* NO_TEXT = "No";
}

// Stores the stream to print to; no block has been printed yet.
ResultPrinter::ResultPrinter(std::ostream& outputStream)
    : output(outputStream), firstBlock(true) {}

// Prints a complete four-line result block.
void ResultPrinter::printResult(const std::string& keyLabel, const std::string& keyValue, bool success, const std::string& message, const std::string& outcomeLabel) {
    beginBlock();
    printLine(keyLabel + ": " + keyValue);
    printLine(std::string("Status: ") + (success ? SUCCESS_TEXT : FAILURE_TEXT));
    printLine("Message: " + message);
    printLine(outcomeLabel + ": " + (success ? YES_TEXT : NO_TEXT));
}

// Separates this block from the previous one with a blank line.
void ResultPrinter::beginBlock() {
    if (!firstBlock) {
        output << "\n";
    }
    firstBlock = false;
}

// Writes one line of the current block.
void ResultPrinter::printLine(const std::string& line) {
    output << line << "\n";
}