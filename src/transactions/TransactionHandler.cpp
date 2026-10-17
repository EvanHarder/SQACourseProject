
/*
 * TransactionHandler.cpp
 *
 * Implements the transaction dispatching structure for the
 * Digital Games Distribution System.
 *
 * Account and game operations will be connected when the
 * data classes are available.
 */

#include "transactions/TransactionHandler.h"

#include <cmath>      // std::llround
#include <cstddef>    // std::size_t
#include <cstdlib>    // std::strtod
#include <iomanip>    // std::setprecision
#include <sstream>    // std::ostringstream
#include <string>
#include <vector>     // used by handleList once the data classes exist

// File-local constants and helpers. Not part of the class, so they are not
// declared in TransactionHandler.h.
namespace {

    // ---- Limits from the handout ----
    const std::size_t MAX_USERNAME_LENGTH  = 15;
    const std::size_t MAX_GAME_NAME_LENGTH = 25;
    const double MAX_GAME_PRICE      = 999.99;     // highest price for a game
    const double MAX_CREDIT          = 999999.00;  // highest account balance
    const double MAX_SESSION_CREDIT  = 1000.00;    // most addable per account per session

    // Converts what the admin typed (a word or a code) into the two-letter
    // type code. Returns an empty string if the type is not recognised.
    std::string toTypeCode(const std::string& text) {
        if (text == "admin" || text == "AA")           return "AA";
        if (text == "full-standard" || text == "FS")   return "FS";
        if (text == "buy-standard" || text == "BS")    return "BS";
        if (text == "sell-standard" || text == "SS")   return "SS";
        return "";
    }

    std::string typeNameFor(const std::string& code) {
        if (code == "AA") return "admin";
        if (code == "FS") return "full-standard";
        if (code == "BS") return "buy-standard";
        if (code == "SS") return "sell-standard";
        return code;
    }

    // Converts price text such as "15" or "15.00" into a number. Accepts only
    // digits with an optional decimal point and at most two decimal places,
    // so negatives, letters and "1.234" are all rejected.
    // Returns false if the text is not a valid price.
    bool parsePrice(const std::string& text, double& price) {
        if (text.empty()) {
            return false;
        }

        bool seenPoint = false;
        std::size_t digitsAfterPoint = 0;
        std::size_t digitCount = 0;

        for (char character : text) {
            if (character == '.') {
                if (seenPoint) {
                    return false;           // two decimal points
                }
                seenPoint = true;
            }
            else if (character >= '0' && character <= '9') {
                digitCount++;
                if (seenPoint) {
                    digitsAfterPoint++;
                }
            }
            else {
                return false;               // letter, sign, space, etc.
            }
        }

        if (digitCount == 0 || digitsAfterPoint > 2) {
            return false;
        }

        price = std::strtod(text.c_str(), nullptr);
        return true;
    }

    // Converts dollars to whole cents so that limit checks are exact
    // (0.1 + 0.2 style rounding errors can't push a total over a limit).
    long long toCents(double dollars) {
        return static_cast<long long>(std::llround(dollars * 100.0));
    }

    // Formats a dollar amount with two decimals, e.g. 600 -> "600.00".
    // Temporary: replace with Evan's FormatUtils once it exists.
    std::string formatMoney(double amount) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(2) << amount;
        return out.str();
    }

    // Outcome label shown on the last line of a result block for each
    // transaction. Unknown codes get a generic label.
    std::string outcomeLabelFor(const std::string& code) {
        if (code == "login")        return "Logged In";
        if (code == "logout")       return "Logged Out";
        if (code == "create")       return "Created";
        if (code == "delete")       return "Deleted";
        if (code == "sell")         return "ForSale";
        if (code == "buy")          return "Game Purchased";
        if (code == "refund")       return "Credit Refunded";
        if (code == "addcredit")    return "Credit Added";
        if (code == "list")         return "Games Listed";
        if (code == "listaccounts") return "Accounts Listed";
        return "Transaction Accepted";
    }

}  

// Stores references to the input reader, result printer, and current session.
TransactionHandler::TransactionHandler(
    InputReader& reader,
    ResultPrinter& printer,
    Session& currentSession
    //UserAccounts& accounts,
    //AvailableGames& games,
    //GameCollection& collection
    //, TransactionLog& log
    )
    : inputReader(reader),
      resultPrinter(printer),
      session(currentSession)
      //userAccounts(accounts),
      //availableGames(games),
      //gameCollection(collection)
      // , transactionLog(log)
{
}

// Routes a transaction code to its corresponding handler.
void TransactionHandler::handleTransaction(
    const std::string& transactionCode) {

    if (transactionCode != "login" && !session.isLoggedIn()) {
        resultPrinter.printResult(
            "Transaction", transactionCode, false,
            "You must log in first.", outcomeLabelFor(transactionCode));
        return;
    }

    if (transactionCode != "login" && !isPermitted(transactionCode)) {
        resultPrinter.printResult(
            "Transaction", transactionCode, false,
            "Permission denied.", outcomeLabelFor(transactionCode));
        return;
    }

    if (transactionCode == "login") {
        handleLogin();
    }
    else if (transactionCode == "logout") {
        handleLogout();
    }
    else if (transactionCode == "create") {
        handleCreate();
    }
    else if (transactionCode == "delete") {
        handleDelete();
    }
    else if (transactionCode == "sell") {
        handleSell();
    }
    else if (transactionCode == "buy") {
        handleBuy();
    }
    else if (transactionCode == "refund") {
        handleRefund();
    }
    else if (transactionCode == "addcredit") {
        handleAddCredit();
    }
    else if (transactionCode == "list") {
        handleList();
    }
    else if (transactionCode == "listaccounts") {
        handleListAccounts();
    }
    else {
        resultPrinter.printResult(
            "Transaction", transactionCode, false,
            "Invalid transaction.", outcomeLabelFor(transactionCode));
    }
}

// Prints a temporary failure result until a transaction has its full implementation.
void TransactionHandler::printNotImplemented(
    const std::string& transactionCode) {

    resultPrinter.printResult(
        "Transaction",
        transactionCode,
        false,
        "Transaction handler is not implemented yet.",
        outcomeLabelFor(transactionCode));
}

bool TransactionHandler::isPermitted(const std::string& transactionCode) const {

    // Admin logins accept every transaction.
    if (session.isAdmin()) {
        return true;
    }

    // Privileged transactions: admin only.
    if (transactionCode == "create" || transactionCode == "delete" ||
        transactionCode == "refund" || transactionCode == "listaccounts") {
        return false;
    }

    // Semi-privileged: sell is refused for buy-standard (BS) users.
    if (transactionCode == "sell" && session.getUserType() == "BS") {
        return false;
    }

    // Semi-privileged: buy is refused for sell-standard (SS) users.
    if (transactionCode == "buy" && session.getUserType() == "SS") {
        return false;
    }

    // logout, addcredit and list are open to every logged-in user.
    return true;
}

// Transaction Logic

// Handles the login transaction: rejects a second login, loads the data files,
// asks for a username, checks it exists and starts the session.
void TransactionHandler::handleLogin() {

    // Rule: no subsequent login until after a logout.
    if (session.isLoggedIn()) {
        resultPrinter.printResult(
            "Transaction", "login", false,
            "A user is already logged in.", outcomeLabelFor("login"));
        return;
    }

    // Rule: the current user accounts file is read before login is processed.
    // if (!userAccounts.loadFromFile()) {
    //     resultPrinter.printResult(
    //         "Transaction", "login", false,
    //         "Could not read the user accounts file.", outcomeLabelFor("login"));
    //     return;
    // }

    // Rule: ask for the username.
    std::string username;
    ReadStatus status = inputReader.readField("username", username);

    if (status == ReadStatus::EndOfInput) {
        resultPrinter.printResult(
            "Transaction", "login", false,
            "Input ended before a username was given.", outcomeLabelFor("login"));
        return;
    }
    if (status == ReadStatus::BadFormat) {
        resultPrinter.printResult(
            "Input", username, false,
            "Expected a line of the form 'username: <name>'.", outcomeLabelFor("login"));
        return;
    }

    // The username must belong to an existing account.
    // User* user = userAccounts.findUser(username);
    // if (user == nullptr) {
    //     resultPrinter.printResult(
    //         "Username", username, false,
    //         "Unknown username.", outcomeLabelFor("login"));
    //     return;
    // }

    // Rule: once the username is accepted, read the available games file.
    // The collection file is read too, because buy must check what the user owns.
    // if (!availableGames.loadFromFile() || !gameCollection.loadFromFile()) {
    //     resultPrinter.printResult(
    //         "Username", username, false,
    //         "Could not read the games files.", outcomeLabelFor("login"));
    //     return;
    // }

    // Start accepting transactions; the type code (AA, FS, BS, SS) sets privileges.
    // session.start(user->username, user->permissions);

    // resultPrinter.printResult(
    //     "Username", username, true,
    //     "Welcome " + user->username + ". Account type: " +
    //     typeNameFor(user->permissions) + ". Credit: $" +
    //     formatMoney(user->balance) + ".",
    //     outcomeLabelFor("login"));

    // TEMPORARY: lets you test the other transactions before the data classes
    // are connected. Treats every username as an admin. Delete these lines
    // when you uncomment the block above.
    
    session.start(username, "AA");
    resultPrinter.printResult(
        "Username", username, true,
        "Login successful.", outcomeLabelFor("login"));
}

void TransactionHandler::handleLogout() {
    if (!session.isLoggedIn()) {
        resultPrinter.printResult(
            "Transaction", "logout", false,
            "No user is currently logged in.", outcomeLabelFor("logout"));
        return;
    }

    std::string username = session.getUsername();

    // Write out the daily transaction file.
    // User* user = userAccounts.findUser(username);

    // TODO: needs Evan's TransactionLog to support the 00 end-of-session line.
    // transactionLog.recordEndOfSession(
    //     username, session.getUserType(), user->balance);
    //
    // if (!transactionLog.outputTransactions()) {
    //     resultPrinter.printResult(
    //         "Username", username, false,
    //         "Could not write the daily transaction file.", outcomeLabelFor("logout"));
    //     return;
    // }

    session.end();

    resultPrinter.printResult(
        "Transaction", "logout", true,
        "Logout successful. Session transactions saved.",
        outcomeLabelFor("logout"));
    }

void TransactionHandler::handleCreate() {

    if (!session.isAdmin()) {
        resultPrinter.printResult(
            "Transaction", "create", false,
            "Permission denied.", outcomeLabelFor("create"));
        return;
    }

    // Rule: ask for the new username.
    std::string newUsername;
    ReadStatus status = inputReader.readField("username", newUsername);

    if (status == ReadStatus::EndOfInput) {
        resultPrinter.printResult(
            "Transaction", "create", false,
            "Input ended before a username was given.", outcomeLabelFor("create"));
        return;
    }

    if (status == ReadStatus::BadFormat) {
        resultPrinter.printResult(
            "Input", newUsername, false,
            "Expected a line of the form 'username: <name>'.", outcomeLabelFor("create"));
        return;
    }

    // Rule: the new username is at most 15 characters (and not empty).
    if (newUsername.empty() || newUsername.size() > MAX_USERNAME_LENGTH) {
        resultPrinter.printResult(
            "Username", newUsername, false,
            "Username must be 1 to 15 characters long.", outcomeLabelFor("create"));
        return;
    }

    // Rule: the new username must differ from every current user.
    // "END" is also refused because it marks the end of the accounts file.
    // if (newUsername == "END" || userAccounts.findUser(newUsername) != nullptr) {
    //     resultPrinter.printResult(
    //         "Username", newUsername, false,
    //         "That username is already taken.", outcomeLabelFor("create"));
    //     return;
    // }

    // Rule: ask for the type of user (admin, full-standard, buy-standard,
    // sell-standard).
    std::string typeText;
    status = inputReader.readField("type", typeText);

    if (status == ReadStatus::EndOfInput) {
        resultPrinter.printResult(
            "Username", newUsername, false,
            "Input ended before a user type was given.", outcomeLabelFor("create"));
        return;
    }

    if (status == ReadStatus::BadFormat) {
        resultPrinter.printResult(
            "Input", typeText, false,
            "Expected a line of the form 'type: <user type>'.", outcomeLabelFor("create"));
        return;
    }

    std::string typeCode = toTypeCode(typeText);
    if (typeCode.empty()) {
        resultPrinter.printResult(
            "Username", newUsername, false,
            "Type must be admin, full-standard, buy-standard or sell-standard.",
            outcomeLabelFor("create"));
        return;
    }

    // userAccounts.addUser(newUsername, typeCode, 0.0);

    // Rule: save this information to the daily transaction file (code 01).

    // transactionLog.recordCreate(newUsername, typeCode, 0.0);

    resultPrinter.printResult(
        "Username", newUsername, true,
        "User created.", outcomeLabelFor("create"));

}

void TransactionHandler::handleDelete() {

    if (!session.isAdmin()) {
        resultPrinter.printResult(
            "Transaction", "delete", false,
            "Permission denied.", outcomeLabelFor("delete"));
        return;
    }

    std::string targetUsername;
    ReadStatus status = inputReader.readField("username", targetUsername);

    if (status == ReadStatus::EndOfInput) {
        resultPrinter.printResult(
            "Transaction", "delete", false,
            "Input ended before a username was given.", outcomeLabelFor("delete"));
        return;
    }

    if (status == ReadStatus::BadFormat) {
        resultPrinter.printResult(
            "Input", targetUsername, false,
            "Expected a line of the form 'username: <name>'.", outcomeLabelFor("delete"));
        return;
    }

    // Rule: the username must not be the name of the current user.
    if (targetUsername == session.getUsername()) {
        resultPrinter.printResult(
            "Username", targetUsername, false,
            "You cannot delete the account you are logged in with.", outcomeLabelFor("delete"));
        return;
    }

    // Rule: the username must be the name of an existing user. A user already
    // deleted earlier in this session no longer counts as existing.

    // User* user = userAccounts.findUser(targetUsername);
    // if (user == nullptr /* || session.isDeleted(targetUsername) */) {
    //     resultPrinter.printResult(
    //         "Username", targetUsername, false,
    //         "No such user exists.", outcomeLabelFor("delete"));
    //     return;
    // }

    // Copy the details the log line needs BEFORE the account is removed.
    // std::string deletedType = user->permissions;
    // double deletedCredit = user->balance;

    // Rule: cancel any games the user has for sale.
    // availableGames.removeGamesBySeller(targetUsername);

    // Rule: remove the user account.
    // userAccounts.removeUser(targetUsername);

    // Rule: no further transactions on the deleted user's inventory of games.
    // Remembering the name in the session lets buy and listaccounts skip it.
    // session.markDeleted(targetUsername);

    // Rule: save this information to the daily transaction file (code 02).
    // transactionLog.recordDelete(targetUsername, deletedType, deletedCredit);

    resultPrinter.printResult(
        "Username", targetUsername, true,
        "User deleted.", outcomeLabelFor("delete"));
}

void TransactionHandler::handleSell() {

    // Rule: semi-privileged, accepted for every account type except
    // buy-standard (BS). The dispatcher already checks this; it is repeated
    // so the handler is safe on its own.
    if (session.getUserType() == "BS") {
        resultPrinter.printResult(
            "Transaction", "sell", false,
            "Permission denied.", outcomeLabelFor("sell"));
        return;
    }

    // Rule: ask for the game name.
    std::string gameName;
    ReadStatus status = inputReader.readField("gamename", gameName);

    if (status == ReadStatus::EndOfInput) {
        resultPrinter.printResult(
            "Transaction", "sell", false,
            "Input ended before a game name was given.", outcomeLabelFor("sell"));
        return;
    }

    if (status == ReadStatus::BadFormat) {
        resultPrinter.printResult(
            "Input", gameName, false,
            "Expected a line of the form 'gamename: <name>'.", outcomeLabelFor("sell"));
        return;
    }

    // Rule: the game name is at most 25 characters (and not empty).
    if (gameName.empty() || gameName.size() > MAX_GAME_NAME_LENGTH) {
        resultPrinter.printResult(
            "Game", gameName, false,
            "Game name must be 1 to 25 characters long.", outcomeLabelFor("sell"));
        return;
    }

    // Rule: all games must have unique names. "END" is also refused because
    // it marks the end of the available games file.
    // if (gameName == "END" || availableGames.findGameByName(gameName) != nullptr) {
    //     resultPrinter.printResult(
    //         "Game", gameName, false,
    //         "A game with that name already exists.", outcomeLabelFor("sell"));
    //     return;
    // }

    // Rule: ask for the price in dollars (e.g. 15.00).
    std::string priceText;
    status = inputReader.readField("price", priceText);

    if (status == ReadStatus::EndOfInput) {
        resultPrinter.printResult(
            "Game", gameName, false,
            "Input ended before a price was given.", outcomeLabelFor("sell"));
        return;
    }

    if (status == ReadStatus::BadFormat) {
        resultPrinter.printResult(
            "Input", priceText, false,
            "Expected a line of the form 'price: <dollars>'.", outcomeLabelFor("sell"));
        return;
    }

    double price = 0.0;
    if (!parsePrice(priceText, price)) {
        resultPrinter.printResult(
            "Price", priceText, false,
            "Price must be a number in dollars, e.g. 15.00.", outcomeLabelFor("sell"));
        return;
    }

    // Rule: the maximum price for a game is 999.99.
    if (price > MAX_GAME_PRICE) {
        resultPrinter.printResult(
            "Price", priceText, false,
            "Price cannot be more than 999.99.", outcomeLabelFor("sell"));
        return;
    }

    // Apply the change: the seller is the logged-in user. Adding the game to
    // the in-memory list also makes a second sell of the same name in this
    // session fail the uniqueness check above.
    // availableGames.addGame(gameName, session.getUsername(), price);

    // Rule: no further transactions on a new game until the next session.
    // Remembering the name in the session lets buy refuse it.
    // session.markNewGame(gameName);

    // Rule: save this information to the daily transaction file (code 03).
    // transactionLog.recordSell(gameName, session.getUsername(), price);

    resultPrinter.printResult(
        "Game", gameName, true,
        "Game put up for sale.", outcomeLabelFor("sell"));
}

void TransactionHandler::handleBuy() {
    
    // Rule: semi-privileged, accepted for every account type except
    // sell-standard (SS). The dispatcher already checks this; it is repeated
    // so the handler is safe on its own.
    if (session.getUserType() == "SS") {
        resultPrinter.printResult(
            "Transaction", "buy", false,
            "Permission denied.", outcomeLabelFor("buy"));
        return;
    }

    // Rule: ask for the game name.
    std::string gameName;
    ReadStatus status = inputReader.readField("gamename", gameName);

    if (status == ReadStatus::EndOfInput) {
        resultPrinter.printResult(
            "Transaction", "buy", false,
            "Input ended before a game name was given.", outcomeLabelFor("buy"));
        return;
    }

    if (status == ReadStatus::BadFormat) {
        resultPrinter.printResult(
            "Input", gameName, false,
            "Expected a line of the form 'gamename: <name>'.", outcomeLabelFor("buy"));
        return;
    }

    // Rule: ask for the seller's username.
    std::string sellerName;
    status = inputReader.readField("seller", sellerName);

    if (status == ReadStatus::EndOfInput) {
        resultPrinter.printResult(
            "Game", gameName, false,
            "Input ended before a seller was given.", outcomeLabelFor("buy"));
        return;
    }

    if (status == ReadStatus::BadFormat) {
        resultPrinter.printResult(
            "Input", sellerName, false,
            "Expected a line of the form 'seller: <name>'.", outcomeLabelFor("buy"));
        return;
    }

    // Rule: the game name must be an existing game.
    // const Game* game = availableGames.findGameByName(gameName);
    // if (game == nullptr) {
    //     resultPrinter.printResult(
    //         "Game", gameName, false,
    //         "No such game is for sale.", outcomeLabelFor("buy"));
    //     return;
    // }

    // Needs a 'seller' field on Game (see the list for Evan below):
    // if (game->seller != sellerName) { ...same failure message... }

    // Rule: a user cannot buy their own game.
    if (sellerName == session.getUsername()) {
        resultPrinter.printResult(
            "Game", gameName, false,
            "You cannot buy your own game.", outcomeLabelFor("buy"));
        return;
    }

    // Rule: no transactions on a game listed this session, or on a game
    // whose seller was deleted this session. (Needs the Session additions below.)
    // if (session.isNewGame(gameName) || session.isDeleted(sellerName)) {
    //     resultPrinter.printResult(
    //         "Game", gameName, false,
    //         "That game cannot be bought in this session.", outcomeLabelFor("buy"));
    //     return;
    // }

    // User* buyer  = userAccounts.findUser(session.getUsername());
    // User* seller = userAccounts.findUser(sellerName);
    // if (buyer == nullptr || seller == nullptr) {
    //     resultPrinter.printResult(
    //         "Game", gameName, false,
    //         "Buyer and seller must be current users.", outcomeLabelFor("buy"));
    //     return;
    // }

    // Rule: the buyer must have enough money to buy the game.
    // double price = game->price;
    // if (toCents(buyer->balance) < toCents(price)) {
    //     resultPrinter.printResult(
    //         "Game", gameName, false,
    //         "Not enough credit to buy this game.", outcomeLabelFor("buy"));
    //     return;
    // }

    // Rule: the buyer must not already have a copy in their collection.
    // if (gameCollection.userOwnsGame(session.getUsername(), gameName)) {
    //     resultPrinter.printResult(
    //         "Game", gameName, false,
    //         "You already own this game.", outcomeLabelFor("buy"));
    //     return;
    // }

    // Rule: the seller's balance must not go over the 999,999.00 maximum.
    // if (toCents(seller->balance) + toCents(price) > toCents(MAX_CREDIT)) {
    //     resultPrinter.printResult(
    //         "Game", gameName, false,
    //         "Seller credit would exceed 999999.00.", outcomeLabelFor("buy"));
    //     return;
    // }

    // Rule: add the game to the buyer's collection, credit the seller and
    // deduct the same amount from the buyer.
    // gameCollection.addGameToUser(session.getUsername(), gameName);
    // userAccounts.updateBalance(sellerName, seller->balance + price);
    // userAccounts.updateBalance(session.getUsername(), buyer->balance - price);

    // Rule: save this information to the daily transaction file (code 04).
    // transactionLog.recordBuy(gameName, sellerName, session.getUsername(), price);

    resultPrinter.printResult(
        "Game", gameName, true,
        "Game purchased.", outcomeLabelFor("buy"));
}


void TransactionHandler::handleRefund() {

    if (!session.isAdmin()) {
        resultPrinter.printResult(
            "Transaction", "refund", false,
            "Permission denied.", outcomeLabelFor("refund"));
        return;
    }

    // Rule: ask for the buyer's username.
    std::string buyerName;
    ReadStatus status = inputReader.readField("buyer", buyerName);

    if (status == ReadStatus::EndOfInput) {
        resultPrinter.printResult(
            "Transaction", "refund", false,
            "Input ended before a buyer was given.", outcomeLabelFor("refund"));
        return;
    }

    if (status == ReadStatus::BadFormat) {
        resultPrinter.printResult(
            "Input", buyerName, false,
            "Expected a line of the form 'buyer: <name>'.", outcomeLabelFor("refund"));
        return;
    }

    // Rule: ask for the seller's username.
    std::string sellerName;
    status = inputReader.readField("seller", sellerName);

    if (status == ReadStatus::EndOfInput) {
        resultPrinter.printResult(
            "Buyer", buyerName, false,
            "Input ended before a seller was given.", outcomeLabelFor("refund"));
        return;
    }

    if (status == ReadStatus::BadFormat) {
        resultPrinter.printResult(
            "Input", sellerName, false,
            "Expected a line of the form 'seller: <name>'.", outcomeLabelFor("refund"));
        return;
    }

    // Rule: ask for the amount of credit to transfer.
    std::string amountText;
    status = inputReader.readField("amount", amountText);

    if (status == ReadStatus::EndOfInput) {
        resultPrinter.printResult(
            "Buyer", buyerName, false,
            "Input ended before an amount was given.", outcomeLabelFor("refund"));
        return;
    }

    if (status == ReadStatus::BadFormat) {
        resultPrinter.printResult(
            "Input", amountText, false,
            "Expected a line of the form 'amount: <dollars>'.", outcomeLabelFor("refund"));
        return;
    }

    // Test case: buyer and seller must be different users.
    if (buyerName == sellerName) {
        resultPrinter.printResult(
            "Buyer", buyerName, false,
            "Buyer and seller must be different users.", outcomeLabelFor("refund"));
        return;
    }

    // Test case: the amount must be a positive number of dollars with at most
    // two decimal places. Letters, negatives, zero and "1.234" are rejected.
    double amount = 0.0;
    if (!parsePrice(amountText, amount) || amount <= 0.0) {
        resultPrinter.printResult(
            "Buyer", buyerName, false,
            "Invalid amount.", outcomeLabelFor("refund"));
        return;
    }

    // Rule: buyer and seller both must be current users.
    // User* buyer = userAccounts.findUser(buyerName);
    // if (buyer == nullptr /* || session.isDeleted(buyerName) */) {
    //     resultPrinter.printResult(
    //         "Buyer", buyerName, false,
    //         "Buyer must be a current user.", outcomeLabelFor("refund"));
    //     return;
    // }

    // User* seller = userAccounts.findUser(sellerName);
    // if (seller == nullptr /* || session.isDeleted(sellerName) */) {
    //     resultPrinter.printResult(
    //         "Buyer", buyerName, false,
    //         "Seller must be a current user.", outcomeLabelFor("refund"));
    //     return;
    // }

    // The seller must have enough credit to give.
    // if (toCents(seller->balance) < toCents(amount)) {
    //     resultPrinter.printResult(
    //         "Buyer", buyerName, false,
    //         "Seller has insufficient credit.", outcomeLabelFor("refund"));
    //     return;
    // }

    // The buyer's balance must not go over the maximum.
    // if (toCents(buyer->balance) + toCents(amount) > toCents(MAX_CREDIT)) {
    //     resultPrinter.printResult(
    //         "Buyer", buyerName, false,
    //         "Buyer credit would exceed 999999.00.", outcomeLabelFor("refund"));
    //     return;
    // }

    // Rule: transfer the credit from the seller to the buyer.
    // userAccounts.updateBalance(sellerName, seller->balance - amount);
    // userAccounts.updateBalance(buyerName, buyer->balance + amount);

    // Rule: save this information to the daily transaction file (code 05).
    // transactionLog.recordRefund(buyerName, sellerName, amount);

    resultPrinter.printResult(
        "Buyer", buyerName, true,
        "Credit refunded.", outcomeLabelFor("refund"));
}

// Handles the addcredit transaction: asks for an amount (and, for an admin,
// the account to credit), validates it, then adds the credit and records it.
void TransactionHandler::handleAddCredit() {

    // Rule: ask for the amount of credit to add.
    std::string amountText;
    ReadStatus status = inputReader.readField("amount", amountText);

    if (status == ReadStatus::EndOfInput) {
        resultPrinter.printResult(
            "Transaction", "addcredit", false,
            "Input ended before an amount was given.", outcomeLabelFor("addcredit"));
        return;
    }
    if (status == ReadStatus::BadFormat) {
        resultPrinter.printResult(
            "Input", amountText, false,
            "Expected a line of the form 'amount: <dollars>'.", outcomeLabelFor("addcredit"));
        return;
    }

    // Rule: in admin mode, also ask for the username of the account to credit.
    // In a standard account the credit always goes to the logged-in user.
    // The username line is read before the amount is validated so that a bad
    // amount does not leave the username line behind to be read as a
    // transaction code.
    std::string targetName = session.getUsername();

    if (session.isAdmin()) {
        status = inputReader.readField("username", targetName);

        if (status == ReadStatus::EndOfInput) {
            resultPrinter.printResult(
                "Amount", amountText, false,
                "Input ended before a username was given.", outcomeLabelFor("addcredit"));
            return;
        }
        if (status == ReadStatus::BadFormat) {
            resultPrinter.printResult(
                "Input", targetName, false,
                "Expected a line of the form 'username: <name>'.", outcomeLabelFor("addcredit"));
            return;
        }
    }

    // Test cases: "abc", "-50" and "10.555" are all rejected by parsePrice;
    // zero is rejected because adding nothing is meaningless.
    double amount = 0.0;
    if (!parsePrice(amountText, amount) || amount <= 0.0) {
        resultPrinter.printResult(
            "Amount", amountText, false,
            "Invalid amount.", outcomeLabelFor("addcredit"));
        return;
    }

    // Rule: the username must be an existing user.
    // User* target = userAccounts.findUser(targetName);
    // if (target == nullptr /* || session.isDeleted(targetName) */) {
    //     resultPrinter.printResult(
    //         "Amount", amountText, false,
    //         "Username not found.", outcomeLabelFor("addcredit"));
    //     return;
    // }

    // Rule: at most $1000.00 per account per session. (Needs the Session additions below.)
    // if (session.getCreditAddedCents(targetName) + toCents(amount)
    //         > toCents(MAX_SESSION_CREDIT)) {
    //     resultPrinter.printResult(
    //         "Amount", amountText, false,
    //         "Session limit of $1000.00 exceeded for this account.",
    //         outcomeLabelFor("addcredit"));
    //     return;
    // }

    // Rule: an account balance can never go over 999,999.00.
    // if (toCents(target->balance) + toCents(amount) > toCents(MAX_CREDIT)) {
    //     resultPrinter.printResult(
    //         "Amount", amountText, false,
    //         "Credit would exceed 999999.00.", outcomeLabelFor("addcredit"));
    //     return;
    // }

    // Apply the change (the pointer sees the updated balance afterwards).
    // userAccounts.updateBalance(targetName, target->balance + amount);
    // session.recordCreditAdded(targetName, toCents(amount));

    // Rule: save this information to the daily transaction file (code 06).
    // transactionLog.recordAddCredit(targetName, target->permissions, target->balance);

    resultPrinter.printResult(
            "Amount", amountText, true,
            "Added $" + formatMoney(amount) + " to " + targetName + ".",
            outcomeLabelFor("addcredit"));
}


void TransactionHandler::handleList() {

    // const std::vector<Game>& games = availableGames.getAllGames();

    // std::string message = std::to_string(games.size()) +
    //     (games.size() == 1 ? " game" : " games") + " available for sale.";

    // for (const Game& game : games) {
    //     message += "\nGame: " + game.name +
    //                // " | Seller: " + game.seller +   // enable once Game has 'seller'
    //                " | Price: $" + formatMoney(game.price);
    // }

    // resultPrinter.printResult(
    //     "Transaction", "list", true, message, outcomeLabelFor("list"));

    printNotImplemented("list");
}

// Handles the listaccounts transaction: prints every active account with its
// username, user type and credit. Admin only. Takes no input fields and
// changes nothing, so it is not written to the daily transaction file.
void TransactionHandler::handleListAccounts() {

    // Rule: privileged transaction, admin only. The dispatcher already checks
    // this; it is repeated so the handler is safe on its own.
    if (!session.isAdmin()) {
        resultPrinter.printResult(
            "Transaction", "listaccounts", false,
            "Permission denied.", outcomeLabelFor("listaccounts"));
        return;
    }

    // const std::vector<User>& users = userAccounts.getUsers();

    // std::string lines;
    // std::size_t count = 0;
    // for (const User& user : users) {
    //     // if (session.isDeleted(user.username)) { continue; }   // needs Session additions
    //     lines += "\nAccount: " + user.username +
    //              " | Type: " + typeNameFor(user.permissions) +
    //              " | Credit: $" + formatMoney(user.balance);
    //     count++;
    // }

    // std::string message = std::to_string(count) +
    //     (count == 1 ? " account" : " accounts") + " active." + lines;

    // resultPrinter.printResult(
    //     "Transaction", "listaccounts", true, message, outcomeLabelFor("listaccounts"));

    printNotImplemented("listaccounts");
}