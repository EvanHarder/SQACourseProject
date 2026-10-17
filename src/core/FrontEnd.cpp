/*
 * FrontEnd.cpp
 *
 * Implementation of the FrontEnd class: the main transaction loop.
 * See FrontEnd.h for the description of each method.
 */

#include "core/FrontEnd.h"

#include <iostream>

namespace {
    const char* TRANSACTION_LABEL = "transaction";
    const char* ACCEPTED_OUTCOME = "Transaction Accepted";
}

// Opens nothing yet: the data files are loaded by the handler at login.
FrontEnd::FrontEnd(const std::string& accountsFilePath, const std::string& availableGamesFilePath, const std::string& gameCollectionFilePath, const std::string& dailyTransactionFilePath, InputReader& inputReader)
    : reader(inputReader), printer(std::cout), session(), accounts(accountsFilePath), games(availableGamesFilePath), collection(gameCollectionFilePath), log(dailyTransactionFilePath), handler(inputReader, printer, session, accounts, games, collection, log) {}

// Reads one "transaction:" line at a time and hands the code to the handler.
void FrontEnd::run() {
    std::string code;
    while (true) {
        ReadStatus status = reader.readField(TRANSACTION_LABEL, code);
        if (status == ReadStatus::EndOfInput) {
            break;
        }
        if (status == ReadStatus::BadFormat) {
            printer.printResult("Line", code, false, "Invalid input format.", ACCEPTED_OUTCOME);
            continue;
        }
        handler.handleTransaction(code);
    }
}