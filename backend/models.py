"""Simple data objects used to represent backend account and game records."""

MAX_CREDIT_CENTS = 99999999  # $999,999.99, the largest credit the accounts file can hold


class User:
    """An account with a username, access type, and credit balance in cents."""

    def __init__(self, username, user_type, credit_cents):
        self.username = username
        self.user_type = user_type
        self.credit_cents = credit_cents

    def can_afford(self, amount_cents):
        """Return True if the user's credit covers the given amount."""
        return self.credit_cents >= amount_cents

    def add_credit(self, amount_cents):
        """Add the given amount to the user's credit, capped at the maximum."""
        self.credit_cents = min(self.credit_cents + amount_cents, MAX_CREDIT_CENTS)

    def remove_credit(self, amount_cents):
        """Subtract the given amount from the user's credit."""
        self.credit_cents -= amount_cents


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