/*
 * InputReader.cpp
 *
 * Implementation of the InputReader class: reads "label: value" lines from the
 * transaction stream. See InputReader.h for the description of each method.
 *
 * Owner: Member B
 */

#include "io/InputReader.h"

namespace {
    const char LABEL_SEPARATOR = ':';
    const char* WHITESPACE = " \t\r\n";
}

// Stores the stream to read from; nothing has been read yet.
InputReader::InputReader(std::istream& inputStream)
    : input(inputStream), reachedEnd(false) {}

// Reads one line, splits it at the first colon and checks the label.
ReadStatus InputReader::readField(const std::string& expectedLabel, std::string& value) {
    std::string line;
    if (!std::getline(input, line)) {
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
