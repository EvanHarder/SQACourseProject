// Console.cpp
//
// Method definitions for Console. See Console.h for what the class is for.

#include "Console.h"

// Creates a console that reads lines from input and writes to output.
Console::Console(std::istream& input, std::ostream& output)
    : input(input), output(output), inputRemaining(true) {}

// Prints promptText on its own line, then reads and returns the next input line.
// Returns "" (without printing the prompt) if there is no more input.
std::string Console::prompt(const std::string& promptText) {
    // Skip the prompt if the last read already reached the end of the input, so
    // the output ends with the last message instead of a dangling prompt.
    // eof() only reports what earlier reads found, so it never waits for typing
    // (peek() would, and the prompt would appear only after the user answered).
    if (input.eof()) {
        inputRemaining = false;
        return "";
    }
    print(promptText);

    std::string line;
    // getline only fails when there is nothing left to read. A last line with no
    // newline after it still counts as a line, so it isn't lost.
    if (!std::getline(input, line)) {
        inputRemaining = false;
        return "";
    }

    // Drop the '\r' left by Windows line endings; any other spacing is kept as typed.
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return line;
}

// Prints text on its own line.
void Console::print(const std::string& text) {
    output << text << "\n";
}

// Prints "Error: " followed by message on its own line.
void Console::printError(const std::string& message) {
    print("Error: " + message);
}

// Returns false once a read has found no more input, true before that.
bool Console::hasInput() const {
    return inputRemaining;
}
