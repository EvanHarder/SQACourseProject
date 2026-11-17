"""Simple data objects used to represent backend account and game records."""


class User:
    """An account with a username, access type, and credit balance in cents."""

    def __init__(self, username, user_type, credit_cents):
        self.username = username
        self.user_type = user_type
        self.credit_cents = credit_cents


class Game:
    """A game listed for sale, including its seller and price in cents."""

    def __init__(self, name, seller, price_cents):
        self.name = name
        self.seller = seller
        self.price_cents = price_cents


class OwnedGame:
    """A record of a game owned by a particular user."""

    def __init__(self, game_name, owner):
        self.game_name = game_name
        self.owner = owner
