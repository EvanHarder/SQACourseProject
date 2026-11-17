// ListGamesTransaction.cpp
//
// Method definitions for ListGamesTransaction. See ListGamesTransaction.h for
// what the class is for.

#include "ListGamesTransaction.h"

#include <vector>

#include "Formatter.h"

// Returns the command word "list".
std::string ListGamesTransaction::getName() const {
    return Config::COMMAND_LIST_GAMES;
}

// Prints every game that can be bought, as "name - seller - price", or a
// message if there are none.
void ListGamesTransaction::execute(Session& session, Console& console) {
    std::vector<Game> available = session.getGamesForSale();
    if (available.empty()) {
        console.print("No games for sale.");
        return;
    }
    console.print("Games for sale:");
    for (const Game& game : available) {
        console.print(game.getName() + " - " + game.getSeller() + " - " + Formatter::formatMoney(game.getPrice()));
    }
}
