
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
#include "data/FormatUtils.h"

#include <cmath>      
#include <iomanip>    
#include <sstream>
#include <cstddef>    
#include <cstdlib>    
#include <string>
#include <vector>    
namespace {

    // Limits 
    const std::size_t MAX_USERNAME_LENGTH  = 15;
    const std::size_t MAX_GAME_NAME_LENGTH = 25;
    const double MAX_GAME_PRICE      = 999.99;     // highest price for a game
    const double MAX_CREDIT          = 999999.00;  // highest account balance
    const double MAX_SESSION_CREDIT  = 1000.00;    // most addable per account per session

    // Converts what the admin typed (a word or a code) into the two-letter type code 
    std::string toTypeCode(const std::string& text) {
        if (text == "admin" || text == "AA")           return "AA";
        if (text == "full-standard" || text == "FS")   return "FS";
        if (text == "buy-standard" || text == "BS")    return "BS";
        if (text == "sell-standard" || text == "SS")   return "SS";
        return "";
    }

    // does the opposite of toTypeCode: converts a type code into a human-readable name
    std::string typeNameFor(const std::string& code) {
        if (code == "AA") return "admin";
        if (code == "FS") return "full-standard";
        if (code == "BS") return "buy-standard";
        if (code == "SS") return "sell-standard";
        return code;
    }

    // Converts price text such as "15" or "15.00" into a number.
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
                    return false;          
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
                return false;               
            }
        }

        if (digitCount == 0 || digitsAfterPoint > 2) {
            return false;
        }

        price = FormatUtils::textToMoney(text);
        return true;
    }

    // Converts dollars to whole cents so that limit checks are exact
    long long toCents(double dollars) {
        return static_cast<long long>(std::llround(dollars * 100.0));
    }

    // Formats a dollar amount with two decimals, e.g. 600 -> "600.00".
    std::string formatMoney(double amount) {
        return FormatUtils::moneyToText(amount, 0);
    }

    // Outcome label shown on the last line of a result block for each transaction
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

// Constructor 
TransactionHandler::TransactionHandler(
    InputReader& reader,
    ResultPrinter& printer,
    Session& currentSession,
    UserAccounts& accounts,
    AvailableGames& games,
    GameCollection& collection,
    TransactionLog& log
    )
    : inputReader(reader),
      resultPrinter(printer),
      session(currentSession),
      userAccounts(accounts),
      availableGames(games),
      gameCollection(collection),
      transactionLog(log)
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

// Prints a temporary failure result until a transaction is able to have its full implementation.
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

// Handles the login transaction: rejects a second login, loads the data files
void TransactionHandler::handleLogin() {

    // no subsequent login until after a logout.
    if (session.isLoggedIn()) {
        resultPrinter.printResult(
            "Transaction", "login", false,
            "A user is already logged in.", outcomeLabelFor("login"));
        return;
    }

    // the current user accounts file is read before login is processed.
    if (!userAccounts.loadFromFile()) {
        resultPrinter.printResult(
            "Transaction", "login", false,
            "Could not read the user accounts file.", outcomeLabelFor("login"));
        return;
    }

    // ask for the username.
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
    User* user = userAccounts.findUser(username);
    if (user == nullptr) {
        resultPrinter.printResult(
            "Username", username, false,
            "Unknown username.", outcomeLabelFor("login"));
        return;
    }

    // once the username is accepted, read the available games file.
    if (!availableGames.loadFromFile() || !gameCollection.loadFromFile()) {
        resultPrinter.printResult(
            "Username", username, false,
            "Could not read the games files.", outcomeLabelFor("login"));
        return;
    }

    // Start accepting transactions; the type code (AA, FS, BS, SS) sets privileges.
    session.start(user->username, user->permissions);

    resultPrinter.printResult(
        "Username", username, true,
        "Welcome " + user->username + ". Account type: " +
        typeNameFor(user->permissions) + ". Credit: $" +
        formatMoney(user->balance) + ".",
        outcomeLabelFor("login"));
    
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
    User* user = userAccounts.findUser(username);

    // PLACEHOLDER: needs TransactionLog::recordEndOfSession

    // transactionLog.recordEndOfSession(
    //     username, session.getUserType(), user->balance);
    
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

    // ask for the new username.
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

    // the new username is at most 15 characters (and not empty).
    if (newUsername.empty() || newUsername.size() > MAX_USERNAME_LENGTH) {
        resultPrinter.printResult(
            "Username", newUsername, false,
            "Username must be 1 to 15 characters long.", outcomeLabelFor("create"));
        return;
    }

    // the new username must differ from every current user.
    if (newUsername == "END" || userAccounts.findUser(newUsername) != nullptr) {
        resultPrinter.printResult(
            "Username", newUsername, false,
            "That username is already taken.", outcomeLabelFor("create"));
        return;
    }

    // ask for the type of user (admin, full-standard, buy-standard,
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

    //PLACEHOLDER: needs UserAccounts::addUser 
    // userAccounts.addUser(newUsername, typeCode, 0.0);

    // saves this information to the daily transaction file (code 01).
    // PLACEHOLDER: needs TransactionLog::recordCreate
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

    // the username must not be the name of the current user.
    if (targetUsername == session.getUsername()) {
        resultPrinter.printResult(
            "Username", targetUsername, false,
            "You cannot delete the account you are logged in with.", outcomeLabelFor("delete"));
        return;
    }

    //  the username must be the name of an existing user. A user already deleted earlier in this session no longer counts as existing.
    User* user = userAccounts.findUser(targetUsername);
    if (user == nullptr /* || session.isDeleted(targetUsername) */) {
        resultPrinter.printResult(
            "Username", targetUsername, false,
            "No such user exists.", outcomeLabelFor("delete"));
        return;
    }

    // Copy the details the log line needs before  account is removed.
    std::string deletedType = user->permissions;
    double deletedCredit = user->balance;

    // cancel any games the user has for sale.
    // PLACEHOLDER: needs AvailableGames::removeGamesBySeller
    // availableGames.removeGamesBySeller(targetUsername);

    // remove the user account.
    //PLACEHOLDER: needs UserAccounts::removeUser
    // userAccounts.removeUser(targetUsername);

    // no further transactions on the deleted user's inventory of games.
    //PLACEHOLDER: needs Session::markDeleted
    // session.markDeleted(targetUsername);

    // save this information to the daily transaction file (code 02).
    // PLACEHOLDER: needs TransactionLog::recordDelete
    // transactionLog.recordDelete(targetUsername, deletedType, deletedCredit);

    resultPrinter.printResult(
        "Username", targetUsername, true,
        "User deleted.", outcomeLabelFor("delete"));
}

void TransactionHandler::handleSell() {

    // semi-privileged, accepted for every account type except buy-standard (BS). The dispatcher already checks this
        if (session.getUserType() == "BS") {
        resultPrinter.printResult(
            "Transaction", "sell", false,
            "Permission denied.", outcomeLabelFor("sell"));
        return;
    }

    // ask for the game name.
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

    //  the game name is at most 25 characters, and not empty.
    if (gameName.empty() || gameName.size() > MAX_GAME_NAME_LENGTH) {
        resultPrinter.printResult(
            "Game", gameName, false,
            "Game name must be 1 to 25 characters long.", outcomeLabelFor("sell"));
        return;
    }

    // all games must have unique names.
    if (gameName == "END" || availableGames.findGameByName(gameName) != nullptr) {
        resultPrinter.printResult(
            "Game", gameName, false,
            "A game with that name already exists.", outcomeLabelFor("sell"));
        return;
    }

    // ask for the price in dollars (e.g. 15.00).
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

    // the maximum price for a game is 999.99.
    if (price > MAX_GAME_PRICE) {
        resultPrinter.printResult(
            "Price", priceText, false,
            "Price cannot be more than 999.99.", outcomeLabelFor("sell"));
        return;
    }

    // Apply the change: the seller is the logged-in user. 
    // PLACEHOLDER: needs AvailableGames::addGame
    // availableGames.addGame(gameName, session.getUsername(), price);

    // no further transactions on a new game until the next session.
    // Remembering the name in the session lets buy refuse it.
    // PLACEHOLDER: needs Session::markNewGame
    // session.markNewGame(gameName);

    // saves this information to the daily transaction file (code 03).
    // PLACEHOLDER: needs TransactionLog::recordSell
    // transactionLog.recordSell(gameName, session.getUsername(), price);

    resultPrinter.printResult(
        "Game", gameName, true,
        "Game put up for sale.", outcomeLabelFor("sell"));
}

void TransactionHandler::handleBuy() {
    
    // semi-privileged, accepted for every account type except
    // sell-standard (SS). 
    if (session.getUserType() == "SS") {
        resultPrinter.printResult(
            "Transaction", "buy", false,
            "Permission denied.", outcomeLabelFor("buy"));
        return;
    }

    // ask for the game name.
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

    // ask for the seller's username.
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

    // the game name must be an existing game.
    const Game* game = availableGames.findGameByName(gameName);
    if (game == nullptr) {
        resultPrinter.printResult(
            "Game", gameName, false,
            "No such game is for sale.", outcomeLabelFor("buy"));
        return;
    }

   if (game->seller != sellerName) {
       resultPrinter.printResult(
           "Game", gameName, false,
           "No such game is for sale by that seller.", outcomeLabelFor("buy"));
       return;
   }
    // a user cannot buy their own game.
    if (sellerName == session.getUsername()) {
        resultPrinter.printResult(
            "Game", gameName, false,
            "You cannot buy your own game.", outcomeLabelFor("buy"));
        return;
    }

    //no transactions on a game listed this session, or on a game whose seller was deleted this session. 
    // PLACEHOLDER: needs Session::isNewGame and Session::isDeleted 
    // if (session.isNewGame(gameName) || session.isDeleted(sellerName)) {
    //     resultPrinter.printResult(
    //         "Game", gameName, false,
    //         "That game cannot be bought in this session.", outcomeLabelFor("buy"));
    //     return;
    // }

    User* buyer  = userAccounts.findUser(session.getUsername());
    User* seller = userAccounts.findUser(sellerName);
    if (buyer == nullptr || seller == nullptr) {
        resultPrinter.printResult(
            "Game", gameName, false,
            "Buyer and seller must be current users.", outcomeLabelFor("buy"));
        return;
    }

    // the buyer must have enough money to buy the game.
    double price = game->price;
    if (toCents(buyer->balance) < toCents(price)) {
        resultPrinter.printResult(
            "Game", gameName, false,
            "Not enough credit to buy this game.", outcomeLabelFor("buy"));
        return;
    }

    // the buyer must not already have a copy in their collection.
    if (gameCollection.userOwnsGame(session.getUsername(), gameName)) {
        resultPrinter.printResult(
            "Game", gameName, false,
            "You already own this game.", outcomeLabelFor("buy"));
        return;
    }

    // the seller's balance must not go over the 999,999.00 maximum.
    if (toCents(seller->balance) + toCents(price) > toCents(MAX_CREDIT)) {
        resultPrinter.printResult(
            "Game", gameName, false,
            "Seller credit would exceed 999999.00.", outcomeLabelFor("buy"));
        return;
    }

    // add the game to the buyer's collection, credit the seller and deduct the same amount from the buyer.
    gameCollection.addGameToUser(session.getUsername(), gameName);
    userAccounts.updateBalance(sellerName, seller->balance + price);
    userAccounts.updateBalance(session.getUsername(), buyer->balance - price);

    // saves this information to the daily transaction file (code 04).
    // PLACEHOLDER: needs TransactionLog::recordBuy
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

    // ask for the buyer's username.
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

    // ask for the seller's username.
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

    // ask for the amount of credit to transfer.
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

    // buyer and seller must be different users.
    if (buyerName == sellerName) {
        resultPrinter.printResult(
            "Buyer", buyerName, false,
            "Buyer and seller must be different users.", outcomeLabelFor("refund"));
        return;
    }

    // the amount must be a positive number of dollars with at most two decimal places. Letters, negatives, zero and "1.234" are rejected.
    double amount = 0.0;
    if (!parsePrice(amountText, amount) || amount <= 0.0) {
        resultPrinter.printResult(
            "Buyer", buyerName, false,
            "Invalid amount.", outcomeLabelFor("refund"));
        return;
    }

    // buyer and seller both must be current users.
    User* buyer = userAccounts.findUser(buyerName);
    if (buyer == nullptr /* || session.isDeleted(buyerName) */) {
        resultPrinter.printResult(
            "Buyer", buyerName, false,
            "Buyer must be a current user.", outcomeLabelFor("refund"));
        return;
    }

    User* seller = userAccounts.findUser(sellerName);
    if (seller == nullptr /* || session.isDeleted(sellerName) */) {
        resultPrinter.printResult(
            "Buyer", buyerName, false,
            "Seller must be a current user.", outcomeLabelFor("refund"));
        return;
    }

    // The seller must have enough credit to give.
    if (toCents(seller->balance) < toCents(amount)) {
        resultPrinter.printResult(
            "Buyer", buyerName, false,
            "Seller has insufficient credit.", outcomeLabelFor("refund"));
        return;
    }

    // The buyer's balance must not go over the maximum.
    if (toCents(buyer->balance) + toCents(amount) > toCents(MAX_CREDIT)) {
        resultPrinter.printResult(
            "Buyer", buyerName, false,
            "Buyer credit would exceed 999999.00.", outcomeLabelFor("refund"));
        return;
    }

    //  transfer the credit from the seller to the buyer.
    userAccounts.updateBalance(sellerName, seller->balance - amount);
    userAccounts.updateBalance(buyerName, buyer->balance + amount);

    // saves this information to the daily transaction file (code 05).
    // PLACEHOLDER: needs TransactionLog::recordRefund
    // transactionLog.recordRefund(buyerName, sellerName, amount);

    resultPrinter.printResult(
        "Buyer", buyerName, true,
        "Credit refunded.", outcomeLabelFor("refund"));
}

// Handles the addcredit transaction: asks for an amount (and, for an admin, the account to credit), validates it, then adds the credit and records it.
void TransactionHandler::handleAddCredit() {

    // ask for the amount of credit to add.
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

    // in admin mode, also ask for the username of the account to credit.
    // In a standard account the credit always goes to the logged-in user.
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

    // "abc", "-50" and "10.555" are all rejected by parsePrice
    // zero is rejected because adding nothing is meaningless.
    double amount = 0.0;
    if (!parsePrice(amountText, amount) || amount <= 0.0) {
        resultPrinter.printResult(
            "Amount", amountText, false,
            "Invalid amount.", outcomeLabelFor("addcredit"));
        return;
    }

    // the username must be an existing user.
    User* target = userAccounts.findUser(targetName);
    if (target == nullptr /* || session.isDeleted(targetName) */) {
        resultPrinter.printResult(
            "Amount", amountText, false,
            "Username not found.", outcomeLabelFor("addcredit"));
        return;
    }

    // PLACEHOLDER: needs Session::getCreditAddedCents 
    // at most $1000.00 per account per session. 
    // if (session.getCreditAddedCents(targetName) + toCents(amount)
    //         > toCents(MAX_SESSION_CREDIT)) {
    //     resultPrinter.printResult(
    //         "Amount", amountText, false,
    //         "Session limit of $1000.00 exceeded for this account.",
    //         outcomeLabelFor("addcredit"));
    //     return;
    // }

    // an account balance can never go over 999,999.00.
    if (toCents(target->balance) + toCents(amount) > toCents(MAX_CREDIT)) {
        resultPrinter.printResult(
            "Amount", amountText, false,
            "Credit would exceed 999999.00.", outcomeLabelFor("addcredit"));
        return;
    }

    // Apply the change (the pointer sees the updated balance afterwards).
    userAccounts.updateBalance(targetName, target->balance + amount);

    // PLACEHOLDER: needs Session::recordCreditAdded
    // session.recordCreditAdded(targetName, toCents(amount));

    // saves this information to the daily transaction file (code 06).
    // PLACEHOLDER: needs TransactionLog::recordAddCredit 
    // transactionLog.recordAddCredit(targetName, target->permissions, target->balance);

    resultPrinter.printResult(
            "Amount", amountText, true,
            "Added $" + formatMoney(amount) + " to " + targetName + ".",
            outcomeLabelFor("addcredit"));
}


void TransactionHandler::handleList() {

    const std::vector<Game>& games = availableGames.getAllGames();

    std::string message = std::to_string(games.size()) +
        (games.size() == 1 ? " game" : " games") + " available for sale.";

    for (const Game& game : games) {
        message += "\nGame: " + game.name +
                   " | Seller: " + game.seller +   
                   " | Price: $" + formatMoney(game.price);
    }

    resultPrinter.printResult(
        "Transaction", "list", true, message, outcomeLabelFor("list"));

}

// Handles the listaccounts transaction: prints every active account with its username, user type and credit. Admin only. 
void TransactionHandler::handleListAccounts() {

    // privileged transaction, admin only. 
    if (!session.isAdmin()) {
        resultPrinter.printResult(
            "Transaction", "listaccounts", false,
            "Permission denied.", outcomeLabelFor("listaccounts"));
        return;
    }

    const std::vector<User>& users = userAccounts.getUsers();

    std::string lines;
    std::size_t count = 0;
    for (const User& user : users) {
        // if (session.isDeleted(user.username)) { continue; }   // needs Session additions
        lines += "\nAccount: " + user.username +
                 " | Type: " + typeNameFor(user.permissions) +
                 " | Credit: $" + formatMoney(user.balance);
        count++;
    }

    std::string message = std::to_string(count) +
        (count == 1 ? " account" : " accounts") + " active." + lines;

    resultPrinter.printResult(
        "Transaction", "listaccounts", true, message, outcomeLabelFor("listaccounts"));
}