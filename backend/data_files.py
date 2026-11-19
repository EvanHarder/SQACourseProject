# data_files.py
#
# Reads and writes the Back End's three fixed-width data files. Every file
# ends with an END line, and every line must be exactly the file's width:
#
#   accounts    UUUUUUUUUUUUUUU TT CCCCCCCCC              28 characters
#   games       IIIIIIIIIIIIIIIIIIIIIIIIII SSSSSSSSSSSSSSS PPPPPP   49 characters
#   collection  IIIIIIIIIIIIIIIIIIIIIIIIII UUUUUUUUUUUUUUU          42 characters
#
# U username, T user type, C credit (000000.00), I game name, S seller,
# P price (000.00). Text is left-justified and padded with spaces.
#
# Any badly formatted line is a fatal error: the Back End names the file and
# line and stops, so it never works from bad data.
#
# Classes:
#   _DataFile       shared reading, writing and field checks
#   AccountsFile    the current user accounts file
#   GamesFile       the available games file
#   CollectionFile  the game collection file

"""Read and write the backend's fixed-width account, game, and collection files."""

from models import Game, OwnedGame, User
from error_log import ErrorLog


# ---- Line widths, including the END line ----
ACCOUNT_WIDTH = 28
GAME_WIDTH = 49
COLLECTION_WIDTH = 42
END_MARKER = "END"
MAX_USERNAME_LENGTH = 15
MAX_GAME_NAME_LENGTH = 26  # Front End A uses 26, as in the spec's examples

# ---- Field widths ----
# The name widths include the one space that follows the field, so slicing a
# line by these widths gives each field with its separator.
ACCOUNT_NAME_WIDTH = 16   # 15-character username + separator
USER_TYPE_WIDTH = 2
CREDIT_WIDTH = 9          # e.g. 001000.00
GAME_NAME_WIDTH = 27      # 26-character game name + separator
SELLER_WIDTH = 16         # 15-character seller + separator
PRICE_WIDTH = 6           # e.g. 015.00
OWNER_WIDTH = 15          # last field on the line, so no separator

# ---- Money format: dollars, a decimal point, then two digits of cents ----
MONEY_CENTS_WIDTH = 2
MONEY_DECIMAL_POINT_WIDTH = 1
MONEY_SUFFIX_WIDTH = MONEY_CENTS_WIDTH + MONEY_DECIMAL_POINT_WIDTH  # ".00"
MONEY_SCALE = 10 ** MONEY_CENTS_WIDTH                                # 100 cents per dollar
CREDIT_DOLLAR_WIDTH = CREDIT_WIDTH - MONEY_SUFFIX_WIDTH              # 6 dollar digits
PRICE_DOLLAR_WIDTH = PRICE_WIDTH - MONEY_SUFFIX_WIDTH                # 3 dollar digits

# Position of the space between the user type and the credit in an account line
ACCOUNT_SEPARATOR_POSITION = ACCOUNT_NAME_WIDTH + USER_TYPE_WIDTH
ACCOUNT_CREDIT_START = ACCOUNT_SEPARATOR_POSITION + 1

# Zero amounts used on END lines
ZERO_CREDIT = "0" * CREDIT_DOLLAR_WIDTH + "." + "0" * MONEY_CENTS_WIDTH  # 000000.00
ZERO_PRICE = "0" * PRICE_DOLLAR_WIDTH + "." + "0" * MONEY_CENTS_WIDTH    # 000.00

# Admin, full-standard, buy-standard, sell-standard and account manager
VALID_USER_TYPES = ("AA", "FS", "BS", "SS", "AM")  # AM = account manager (Phase 4 requirement change)


class _DataFile:
    """Shared validation and file handling for fixed-width data records.

    Each subclass sets its line width and supplies four methods:
    _is_end (is this the END line), _end_record (the END line to write),
    _parse (one line to an object) and _format (one object to a line).
    """

    width = 0  # line width, set by each subclass

    # Reads every record in the file and returns them as a list.
    # Stops the Back End if the file is missing, a line is the wrong width or
    # badly formatted, or the END line is missing, repeated or not last.
    @classmethod
    def read(cls, path):
        """Load records from a file, requiring a correctly placed END line."""
        try:
            with open(path, "r") as file:
                lines = file.read().splitlines()
        except OSError:
            ErrorLog.fatal(path, "cannot open file")

        items = []
        found_end = False
        for line_number, line in enumerate(lines, start=1):
            if len(line) != cls.width:
                cls._fail(path, line_number, "line must be exactly " + str(cls.width) + " characters")
            if cls._is_end(line):
                if found_end:
                    cls._fail(path, line_number, "duplicate END line")
                if line_number != len(lines):
                    cls._fail(path, line_number, "END must be the last line")
                found_end = True
                continue
            if found_end:
                cls._fail(path, line_number, "record found after END")
            items.append(cls._parse(line, path, line_number))

        if not found_end:
            cls._fail(path, len(lines) + 1, "missing END line")
        return items

    # Writes every record as a fixed-width line, then the END line.
    # A record that can't be written in the file's format is a fatal error.
    @classmethod
    def write(cls, path, items):
        """Write formatted records followed by the file's END line."""
        lines = []
        for line_number, item in enumerate(items, start=1):
            lines.append(cls._format(item, path, line_number))
        lines.append(cls._end_record())
        try:
            with open(path, "w") as file:
                file.write("\n".join(lines) + "\n")
        except OSError:
            ErrorLog.fatal(path, "cannot write file")

    # Stops the Back End with the file name, line number and problem.
    @staticmethod
    def _fail(path, line_number, message):
        """Report a fatal error for one line of a data file."""
        ErrorLog.fatal(path, "line " + str(line_number) + ": " + message)

    # Returns a text field (username, game name, seller or owner) without its padding.
    # The field must not be blank, END, too long, start with a space or hold tabs.
    @staticmethod
    def _text_field(field, path, line_number, label, max_length):
        """Read one padded text field, stopping if it is not valid."""
        value = field.rstrip(" ")
        if (not value or value == END_MARKER or len(value) > max_length
                or value.startswith(" ") or any(char.isspace() and char != " " for char in value)):
            _DataFile._fail(path, line_number, "bad " + label + " field")
        return value

    # Returns a money field ("001000.00" or "015.00") as whole cents.
    # It must be all digits with a decimal point before the last two.
    @staticmethod
    def _money_field(field, path, line_number, label, dollars_width):
        """Read one money field as cents, stopping if it is not valid."""
        expected = dollars_width + MONEY_SUFFIX_WIDTH
        if (len(field) != expected or field[dollars_width] != "."
                or not all("0" <= char <= "9" for char in field[:dollars_width])
                or not all(
                    "0" <= char <= "9"
                    for char in field[dollars_width + MONEY_DECIMAL_POINT_WIDTH:]
                )):
            _DataFile._fail(path, line_number, "bad " + label + " field")
        return int(field[:dollars_width]) * MONEY_SCALE + int(
            field[dollars_width + MONEY_DECIMAL_POINT_WIDTH:]
        )

    # Pads a text value with spaces to the field width, checking it fits first.
    @staticmethod
    def _format_text(value, path, line_number, label, width, max_length):
        """Format one text field for writing, stopping if it is not valid."""
        if (not isinstance(value, str) or not value or value == END_MARKER
                or len(value) > max_length or value.startswith(" ")
                or value.endswith(" ")
                or any(char.isspace() and char != " " for char in value)):
            _DataFile._fail(path, line_number, "bad " + label + " field")
        return value.ljust(width)

    # Turns an amount in cents into fixed-width money text, e.g. 1500 -> "015.00".
    # The amount must be a whole number that is not negative and fits the width.
    @staticmethod
    def _format_money(value, path, line_number, label, width):
        """Format one money field for writing, stopping if it does not fit."""
        if type(value) is not int or value < 0 or value >= 10 ** (width - MONEY_SUFFIX_WIDTH) * MONEY_SCALE:
            _DataFile._fail(path, line_number, "bad " + label + " field")
        dollars, cents = divmod(value, MONEY_SCALE)
        return (str(dollars).zfill(width - MONEY_SUFFIX_WIDTH) + "."
                + str(cents).zfill(MONEY_CENTS_WIDTH))

    # Returns True if the line is an END line: END in the first field, and the
    # rest either blank or the file's zero values.
    @classmethod
    def _end_line(cls, line, field_width, zero_suffix):
        """Check whether a line is the file's END line."""
        first_field = line[:field_width]
        if first_field.rstrip(" ") != END_MARKER:
            return False
        remainder = line[field_width:]
        return remainder.strip(" ") == "" or remainder == zero_suffix


class AccountsFile(_DataFile):
    """Read and write user accounts in the 28-character account format."""

    width = ACCOUNT_WIDTH

    # Returns True for "END" followed by a blank type and 000000.00 (or all blanks).
    @classmethod
    def _is_end(cls, line):
        """Check whether a line is the accounts file's END line."""
        return cls._end_line(line, ACCOUNT_NAME_WIDTH, " " * USER_TYPE_WIDTH + " " + ZERO_CREDIT)

    # Returns the END line written at the end of the accounts file.
    @classmethod
    def _end_record(cls):
        """Build the accounts file's END line."""
        return END_MARKER.ljust(ACCOUNT_NAME_WIDTH) + (" " * USER_TYPE_WIDTH) + " " + ZERO_CREDIT

    # Splits one account line into a User, checking the username, type and credit.
    @classmethod
    def _parse(cls, line, path, line_number):
        """Turn one account line into a User."""
        username = cls._text_field(
            line[:ACCOUNT_NAME_WIDTH], path, line_number, "username", MAX_USERNAME_LENGTH
        )
        user_type = line[ACCOUNT_NAME_WIDTH:ACCOUNT_NAME_WIDTH + USER_TYPE_WIDTH]
        if user_type not in VALID_USER_TYPES:
            cls._fail(path, line_number, "bad user type field")
        if line[ACCOUNT_SEPARATOR_POSITION] != " ":
            cls._fail(path, line_number, "bad account separator")
        credit = cls._money_field(
            line[ACCOUNT_CREDIT_START:ACCOUNT_WIDTH], path, line_number,
            "credit", CREDIT_DOLLAR_WIDTH
        )
        return User(username, user_type, credit)

    # Turns one User into an account line.
    @classmethod
    def _format(cls, user, path, line_number):
        """Turn one User into an account line."""
        username = cls._format_text(
            user.username, path, line_number, "username",
            ACCOUNT_NAME_WIDTH, MAX_USERNAME_LENGTH
        )
        if user.user_type not in VALID_USER_TYPES:
            cls._fail(path, line_number, "bad user type field")
        credit = cls._format_money(user.credit_cents, path, line_number, "credit", CREDIT_WIDTH)
        return username + user.user_type + " " + credit


class GamesFile(_DataFile):
    """Read and write games for sale in the 49-character game format."""

    width = GAME_WIDTH

    # Returns True for "END" followed by a blank seller and 000.00 (or all blanks).
    @classmethod
    def _is_end(cls, line):
        """Check whether a line is the games file's END line."""
        return cls._end_line(line, GAME_NAME_WIDTH, " " * SELLER_WIDTH + ZERO_PRICE)

    # Returns the END line written at the end of the games file.
    @classmethod
    def _end_record(cls):
        """Build the games file's END line."""
        return END_MARKER.ljust(GAME_NAME_WIDTH) + (" " * SELLER_WIDTH) + ZERO_PRICE

    # Splits one game line into a Game, checking the name, seller and price.
    @classmethod
    def _parse(cls, line, path, line_number):
        """Turn one game line into a Game."""
        name = cls._text_field(
            line[:GAME_NAME_WIDTH], path, line_number, "game name", MAX_GAME_NAME_LENGTH
        )
        seller = cls._text_field(
            line[GAME_NAME_WIDTH:GAME_NAME_WIDTH + SELLER_WIDTH],
            path, line_number, "seller", MAX_USERNAME_LENGTH
        )
        price = cls._money_field(
            line[GAME_NAME_WIDTH + SELLER_WIDTH:], path, line_number,
            "price", PRICE_DOLLAR_WIDTH
        )
        return Game(name, seller, price)

    # Turns one Game into a game line.
    @classmethod
    def _format(cls, game, path, line_number):
        """Turn one Game into a game line."""
        name = cls._format_text(
            game.name, path, line_number, "game name", GAME_NAME_WIDTH, MAX_GAME_NAME_LENGTH
        )
        seller = cls._format_text(
            game.seller, path, line_number, "seller", SELLER_WIDTH, MAX_USERNAME_LENGTH
        )
        price = cls._format_money(game.price_cents, path, line_number, "price", PRICE_WIDTH)
        return name + seller + price


class CollectionFile(_DataFile):
    """Read and write ownership records in the 42-character collection format."""

    width = COLLECTION_WIDTH

    # Returns True for "END" followed by a blank owner.
    @classmethod
    def _is_end(cls, line):
        """Check whether a line is the collection file's END line."""
        return cls._end_line(line, GAME_NAME_WIDTH, " " * OWNER_WIDTH)

    # Returns the END line written at the end of the collection file.
    @classmethod
    def _end_record(cls):
        """Build the collection file's END line."""
        return END_MARKER.ljust(COLLECTION_WIDTH)

    # Splits one collection line into an OwnedGame, checking the game name and owner.
    @classmethod
    def _parse(cls, line, path, line_number):
        """Turn one collection line into an OwnedGame."""
        game_name = cls._text_field(
            line[:GAME_NAME_WIDTH], path, line_number, "game name", MAX_GAME_NAME_LENGTH
        )
        owner = cls._text_field(
            line[GAME_NAME_WIDTH:], path, line_number, "owner", MAX_USERNAME_LENGTH
        )
        return OwnedGame(game_name, owner)

    # Turns one OwnedGame into a collection line.
    @classmethod
    def _format(cls, owned_game, path, line_number):
        """Turn one OwnedGame into a collection line."""
        game_name = cls._format_text(
            owned_game.game_name, path, line_number, "game name",
            GAME_NAME_WIDTH, MAX_GAME_NAME_LENGTH
        )
        owner = cls._format_text(
            owned_game.owner, path, line_number, "owner", OWNER_WIDTH, MAX_USERNAME_LENGTH
        )
        return game_name + owner