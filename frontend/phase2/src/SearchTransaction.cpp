// SearchTransaction.cpp
//
// Method definitions for SearchTransaction. See SearchTransaction.h for what
// the class is for and which names are hidden from non-admins.

#include "SearchTransaction.h"

#include <algorithm>
#include <cctype>

#include "Formatter.h"

// Returns the command word "search".
std::string SearchTransaction::getName() const {
    return Config::COMMAND_SEARCH;
}

// Asks for a query and prints the matching games ("name - seller - price",
// like list) and then the matching usernames, or a message for each if none match.
void SearchTransaction::execute(Session& session, Console& console) {
    std::string query = console.prompt("Enter search query:");
    if (query.empty()) {
        console.printError("invalid search query.");
        return;
    }

    std::vector<Game> gamesForSale = session.getGamesForSale();
    std::vector<std::string> matchingGames;
    for (const Game& game : gamesForSale) {
        if (containsIgnoringCase(game.getName(), query)) {
            matchingGames.push_back(game.getName() + " - " + game.getSeller() + " - "
                                    + Formatter::formatMoney(game.getPrice()));
        }
    }

    bool viewerIsAdmin = session.getCurrentUser().isAdmin();
    std::vector<std::string> matchingUsers;
    for (const User& user : session.getUsers()) {
        if (!containsIgnoringCase(user.getName(), query)) {
            continue;
        }
        // Non-admins never see an admin's name unless that admin is selling a game
        if (!viewerIsAdmin && user.isAdmin() && !isSelling(gamesForSale, user.getName())) {
            continue;
        }
        matchingUsers.push_back(user.getName());
    }

    printResults(console, "Games matching \"" + query + "\":", matchingGames, "No games found.");
    printResults(console, "Users matching \"" + query + "\":", matchingUsers, "No users found.");
}

// Returns true if text contains query, ignoring upper/lower case.
bool SearchTransaction::containsIgnoringCase(const std::string& text, const std::string& query) {
    auto sameIgnoringCase = [](char a, char b) {
        return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
    };
    return std::search(text.begin(), text.end(), query.begin(), query.end(), sameIgnoringCase) != text.end();
}

// Returns true if the given user is the seller of at least one of the given games.
bool SearchTransaction::isSelling(const std::vector<Game>& gamesForSale, const std::string& username) {
    for (const Game& game : gamesForSale) {
        if (game.getSeller() == username) {
            return true;
        }
    }
    return false;
}

// Prints the heading, then each line, or noneMessage if there are no lines.
void SearchTransaction::printResults(Console& console, const std::string& heading,
                                     const std::vector<std::string>& lines, const std::string& noneMessage) {
    console.print(heading);
    if (lines.empty()) {
        console.print(noneMessage);
        return;
    }
    for (const std::string& line : lines) {
        console.print(line);
    }
}
