// ListGamesTransaction.h
//
// ListGamesTransaction
// The list command (added by the client): prints every game currently for
// sale with its seller and price. Any logged-in user may run it. Games put up
// for sale this session are left out, since they can't be bought until the
// next session. It writes nothing to the daily transaction file.

#pragma once

#include <string>

#include "Transaction.h"

class ListGamesTransaction : public Transaction {
public:
    // Returns the command word "list".
    std::string getName() const override;

    // Prints every game that can be bought, or a message if there are none.
    void execute(Session& session, Console& console) override;
};
