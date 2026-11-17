// SearchTransaction.h
//
// SearchTransaction
// The search command (added by the client in Phase 3): asks for a search
// query and prints every game for sale and every user whose name contains it.
// Any logged-in user may run it. It writes nothing to the daily transaction file.
//
//   - Matching ignores upper/lower case, so "stardew" finds "Stardew Valley".
//     An empty query is refused, since it would match every name.
//   - Games are the same ones list shows: games put up for sale this session
//     are left out until the next session.
//   - Users are shown by name only (credit stays private, as in listaccounts).
//   - An admin sees every matching user. Any other user does not see the names
//     of admins who have no games for sale (client requirement). An admin who
//     is selling a game is shown, since their name is already on that listing.

#pragma once

#include <string>
#include <vector>

#include "Transaction.h"

class SearchTransaction : public Transaction {
public:
    // Returns the command word "search".
    std::string getName() const override;

    // Asks for a query and prints the matching games and users.
    void execute(Session& session, Console& console) override;

private:
    // Returns true if text contains query, ignoring upper/lower case.
    static bool containsIgnoringCase(const std::string& text, const std::string& query);

    // Returns true if the given user is the seller of at least one of the given games.
    static bool isSelling(const std::vector<Game>& gamesForSale, const std::string& username);

    // Prints the heading, then each line, or noneMessage if there are no lines.
    static void printResults(Console& console, const std::string& heading,
                             const std::vector<std::string>& lines, const std::string& noneMessage);
};
