/*
 * ResultPrinter.h
 *
 * Class: ResultPrinter
 * Intention: Prints the result of each transaction to standard output as one
 * four-line block, with a blank line between blocks:
 *
 *     Username: fullUser
 *     Status: FAILURE
 *     Message: Username already exists.
 *     Account Created: No
 */

#ifndef RESULT_PRINTER_H
#define RESULT_PRINTER_H

#include <ostream>
#include <string>

class ResultPrinter {
public:
    // Creates a printer that writes to the given stream (normally std::cout).
    explicit ResultPrinter(std::ostream& outputStream);

    // Prints the four lines of one result block:
    // "<keyLabel>: <keyValue>", "Status: SUCCESS|FAILURE", "Message: <message>",
    // and "<outcomeLabel>: Yes|No" (Yes when success is true).
    void printResult(const std::string& keyLabel, const std::string& keyValue, bool success, const std::string& message, const std::string& outcomeLabel);

    // Starts a result block made of separate lines; used by list transactions,
    // which print extra lines between the Message and the outcome.
    void beginBlock();

    // Prints one line inside the current block (e.g. "Status: SUCCESS" or a game row).
    void printLine(const std::string& line);

private:
    std::ostream& output;   // where result blocks are written
    bool firstBlock;        // true until the first block is printed (no blank line before it)
};

#endif