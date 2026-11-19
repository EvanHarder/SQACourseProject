# processor.py

# from models import User, Game, OwnedGame

class TransactionProcessor:
    # Applies transactions to the users, games and collection
    def __init__(self, users, games, collection, error_log):
        # Store the data to change and the error log to report to.
        self.users = users
        self.games = games
        self.collection = collection
        self.error_log = error_log

    def process_all(self, transactions):
        # Sends each transaction to the right apply method 
        handlers = {
            "01": self.apply_create,
            "02": self.apply_delete,
            "03": self.apply_sell,
            "04": self.apply_buy,
            "05": self.apply_refund,
            "06": self.apply_addcredit,
        }

        for t in transactions:
            handler = handlers.get(t.code)
            if handler is not None:
                handler(t)

    def apply_create(self, t):
        # Check if the username already exists
        if t.username in self.users:
            self.error_log.constraint("username already exists.")
            return

        # self.users[t.username] = User(t.username, t.user_type, t.credit_cents)

    def apply_delete(self, t):
        # Remove the user, their games for sale and their collection
        if t.username not in self.users:
            self.error_log.constraint("user does not exist.")
            return
        del self.users[t.username]

        # Remove the user's games for sale
        for name in [n for n, g in self.games.items()
                     if g.seller == t.username]:
            del self.games[name]

        # Remove the user's collection
        self.collection[:] = [o for o in self.collection
                              if o.owner != t.username]

    def apply_sell(self, t):
        # Add games to list 
        seller = t.seller if t.seller else t.username
        # self.games[t.game_name] = Game(t.game_name, seller, t.price_cents)

    def apply_buy(self, t):
        # Move credit from buyer to seller and add the game to the buyer's collection
        buyer = self.users.get(t.buyer)
        seller = self.users.get(t.seller)
        if buyer is None or seller is None:
            self.error_log.constraint("Buyer and seller must be current users.")
            return

        # Check if the buyer can afford the game
        if not buyer.can_afford(t.price_cents):
            self.error_log.constraint("Insufficient credit.")
            return

        buyer.remove_credit(t.price_cents)
        seller.add_credit(t.price_cents)
        # self.collection.append(OwnedGame(t.game_name, t.buyer))

    def apply_refund(self, t):
        # Move credit from seller to buyer
        buyer = self.users.get(t.buyer)
        seller = self.users.get(t.seller)
        if buyer is None or seller is None:
            self.error_log.constraint("user does not exist.")
            return
        if not seller.can_afford(t.credit_cents):
            self.error_log.constraint("insufficient credit.")
            return
        seller.remove_credit(t.credit_cents)
        buyer.add_credit(t.credit_cents)

    def apply_addcredit(self, t):
        # Add credit to the user, if they exist
        user = self.users.get(t.username)
        if user is None:
            self.error_log.constraint("user does not exist.")
            return

        user.add_credit(t.credit_cents)