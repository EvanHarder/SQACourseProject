/*
 * InputReader.h
 *
 * Class: InputReader
 * Intention: Reads the transaction stream one line at a time and returns the
 * value from each "label: value" line, checking that the label is the one the
 * Front End is currently asking for.
 *
 * Every line of the input stream has the form   label: value
 * for example   transaction: login   or   username: admin01
 */

#ifndef INPUT_READER_H
#define INPUT_READER_H

#include <istream>
#include <string>

// The three possible results of reading one input line.
enum class ReadStatus {
    Ok,          // the line had the expected label; the value was returned
    BadFormat,   // the line had no label, or a different label than expected
    EndOfInput   // there are no more lines to read
};

class InputReader {
public:
    // Creates a reader that takes its lines from the given stream (normally std::cin).
    explicit InputReader(std::istream& inputStream);

    // Reads the next line and checks that its label matches expectedLabel.
    // On ReadStatus::Ok, value holds the text after the colon with surrounding spaces removed.
    // On ReadStatus::BadFormat, value holds the whole line, so it can be shown in an error.
    // On ReadStatus::EndOfInput, value is set to an empty string.
    ReadStatus readField(const std::string& expectedLabel, std::string& value);

    // Returns true once the end of the input stream has been reached.
    bool atEnd() const;

private:
    // Removes spaces, tabs and carriage returns from both ends of text.
    static std::string trim(const std::string& text);

    // Returns true if text is one or more lower-case letters (a valid label).
    static bool isValidLabel(const std::string& text);

    std::istream& input;   // the stream the transaction lines are read from
    bool reachedEnd;       // set once a read finds no more lines
};

#endif