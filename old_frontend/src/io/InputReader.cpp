/*
 * InputReader.cpp
 *
 * Implementation of the InputReader class: reads "label: value" lines from the
 * transaction stream. See InputReader.h for the description of each method.
 */

#include "io/InputReader.h"

namespace {
    const char* TRANSACTION_LABEL = "transaction";
    const char LABEL_SEPARATOR = ':';
    const char* WHITESPACE = " \t\r\n";
}

// Stores the stream to read from; nothing has been read yet.
InputReader::InputReader(std::istream& inputStream)
    : input(inputStream), reachedEnd(false), hasPendingLine(false) {}

// Reads one line, splits it at the first colon and checks the label.
// A "transaction:" line found while another field is expected is put back, so the next
// transaction is not lost when the current one is missing a field.
ReadStatus InputReader::readField(const std::string& expectedLabel, std::string& value) {
    std::string line;
    if (!nextLine(line)) {
        reachedEnd = true;
        value = "";
        return ReadStatus::EndOfInput;
    }

    std::string trimmedLine = trim(line);
    std::size_t separatorPosition = trimmedLine.find(LABEL_SEPARATOR);
    if (separatorPosition == std::string::npos) {
        value = trimmedLine;
        return ReadStatus::BadFormat;
    }

    std::string label = trimmedLine.substr(0, separatorPosition);
    if (label == TRANSACTION_LABEL && expectedLabel != TRANSACTION_LABEL) {
        hasPendingLine = true;
        pendingLine = line;
        value = trimmedLine;
        return ReadStatus::BadFormat;
    }
    if (!isValidLabel(label) || label != expectedLabel) {
        value = trimmedLine;
        return ReadStatus::BadFormat;
    }

    value = trim(trimmedLine.substr(separatorPosition + 1));
    return ReadStatus::Ok;
}

// Reports whether a previous read has already reached the end of the stream.
bool InputReader::atEnd() const {
    return reachedEnd;
}

// Field lines have a lower-case label followed by a colon.
bool InputReader::isFieldLine(const std::string& line) {
    std::string trimmedLine = trim(line);
    std::size_t separatorPosition = trimmedLine.find(LABEL_SEPARATOR);
    return separatorPosition != std::string::npos && isValidLabel(trimmedLine.substr(0, separatorPosition));
}

// Returns the put-back line first, then lines from the stream.
bool InputReader::nextLine(std::string& line) {
    if (hasPendingLine) {
        hasPendingLine = false;
        line = pendingLine;
        return true;
    }
    return static_cast<bool>(std::getline(input, line));
}

// Strips leading and trailing whitespace (including a Windows '\r').
std::string InputReader::trim(const std::string& text) {
    std::size_t first = text.find_first_not_of(WHITESPACE);
    if (first == std::string::npos) {
        return "";
    }
    std::size_t last = text.find_last_not_of(WHITESPACE);
    return text.substr(first, last - first + 1);
}

// A label is valid only if it is made of lower-case letters a to z.
bool InputReader::isValidLabel(const std::string& text) {
    if (text.empty()) {
        return false;
    }
    for (char character : text) {
        if (character < 'a' || character > 'z') {
            return false;
        }
    }
    return true;
}