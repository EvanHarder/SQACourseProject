// Console.h
//
// Console
// The only class that reads user input or prints to the terminal. Every
// prompt, message and error goes through here, so no other class touches
// std::cin or std::cout.
//
// Input is read one line at a time. When the input runs out, prompt() returns
// "" instead of failing, and hasInput() becomes false so FrontEnd can stop.
// An empty answer is already invalid for every question, so a transaction cut
// off by the end of input fails like any other bad input and the program
// never crashes.
//
// It works on any streams, so the program uses std::cin/std::cout while a test
// could pass in a file or string stream instead.

#pragma once

#include <istream>
#include <ostream>
#include <string>

class Console {
public:
    // Creates a console that reads lines from input and writes to output.
    Console(std::istream& input, std::ostream& output);

    // Prints promptText on its own line, then reads and returns the next input line.
    // Returns "" (without printing the prompt) if there is no more input.
    std::string prompt(const std::string& promptText);

    // Prints text on its own line.
    void print(const std::string& text);

    // Prints "Error: " followed by message on its own line.
    void printError(const std::string& message);

    // Returns false once a read has found no more input, true before that.
    bool hasInput() const;

private:
    std::istream& input;   // where user input is read from (std::cin when running)
    std::ostream& output;  // where prompts and messages go (std::cout when running)
    bool inputRemaining;   // false after a read hits the end of the input
};
