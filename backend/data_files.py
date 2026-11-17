"""Read and write the backend's fixed-width account, game, and collection files."""

from models import Game, OwnedGame, User
from error_log import ErrorLog


ACCOUNT_WIDTH = 28
GAME_WIDTH = 49
COLLECTION_WIDTH = 42
END_MARKER = "END"
MAX_USERNAME_LENGTH = 15
MAX_GAME_NAME_LENGTH = 26

ACCOUNT_NAME_WIDTH = 16
USER_TYPE_WIDTH = 2
CREDIT_WIDTH = 9
GAME_NAME_WIDTH = 27
SELLER_WIDTH = 16
PRICE_WIDTH = 6
OWNER_WIDTH = 15

MONEY_CENTS_WIDTH = 2
MONEY_DECIMAL_POINT_WIDTH = 1
MONEY_SUFFIX_WIDTH = MONEY_CENTS_WIDTH + MONEY_DECIMAL_POINT_WIDTH
MONEY_SCALE = 10 ** MONEY_CENTS_WIDTH
CREDIT_DOLLAR_WIDTH = CREDIT_WIDTH - MONEY_SUFFIX_WIDTH
PRICE_DOLLAR_WIDTH = PRICE_WIDTH - MONEY_SUFFIX_WIDTH

ACCOUNT_SEPARATOR_POSITION = ACCOUNT_NAME_WIDTH + USER_TYPE_WIDTH
ACCOUNT_CREDIT_START = ACCOUNT_SEPARATOR_POSITION + 1

ZERO_CREDIT = "0" * CREDIT_DOLLAR_WIDTH + "." + "0" * MONEY_CENTS_WIDTH
ZERO_PRICE = "0" * PRICE_DOLLAR_WIDTH + "." + "0" * MONEY_CENTS_WIDTH

VALID_USER_TYPES = ("AA", "FS", "BS", "SS")


class _DataFile:
    """Shared validation and file handling for fixed-width data records."""

    width = 0

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

    @staticmethod
    def _fail(path, line_number, message):
        ErrorLog.fatal(path, "line " + str(line_number) + ": " + message)

    @staticmethod
    def _text_field(field, path, line_number, label, max_length):
        value = field.rstrip(" ")
        if (not value or value == END_MARKER or len(value) > max_length
                or value.startswith(" ") or any(char.isspace() and char != " " for char in value)):
            _DataFile._fail(path, line_number, "bad " + label + " field")
        return value

    @staticmethod
    def _money_field(field, path, line_number, label, dollars_width):
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

    @staticmethod
    def _format_text(value, path, line_number, label, width, max_length):
        if (not isinstance(value, str) or not value or value == END_MARKER
                or len(value) > max_length or value.startswith(" ")
                or value.endswith(" ")
                or any(char.isspace() and char != " " for char in value)):
            _DataFile._fail(path, line_number, "bad " + label + " field")
        return value.ljust(width)

    @staticmethod
    def _format_money(value, path, line_number, label, width):
        if type(value) is not int or value < 0 or value >= 10 ** (width - MONEY_SUFFIX_WIDTH) * MONEY_SCALE:
            _DataFile._fail(path, line_number, "bad " + label + " field")
        dollars, cents = divmod(value, MONEY_SCALE)
        return (str(dollars).zfill(width - MONEY_SUFFIX_WIDTH) + "."
                + str(cents).zfill(MONEY_CENTS_WIDTH))

    @classmethod
    def _end_line(cls, line, field_width, zero_suffix):
        first_field = line[:field_width]
        if first_field.rstrip(" ") != END_MARKER:
            return False
        remainder = line[field_width:]
        return remainder.strip(" ") == "" or remainder == zero_suffix


class AccountsFile(_DataFile):
    """Read and write user accounts in the 28-character account format."""

    width = ACCOUNT_WIDTH

    @classmethod
    def _is_end(cls, line):
        return cls._end_line(line, ACCOUNT_NAME_WIDTH, " " * USER_TYPE_WIDTH + " " + ZERO_CREDIT)

    @classmethod
    def _end_record(cls):
        return END_MARKER.ljust(ACCOUNT_NAME_WIDTH) + (" " * USER_TYPE_WIDTH) + " " + ZERO_CREDIT

    @classmethod
    def _parse(cls, line, path, line_number):
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

    @classmethod
    def _format(cls, user, path, line_number):
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

    @classmethod
    def _is_end(cls, line):
        return cls._end_line(line, GAME_NAME_WIDTH, " " * SELLER_WIDTH + ZERO_PRICE)

    @classmethod
    def _end_record(cls):
        return END_MARKER.ljust(GAME_NAME_WIDTH) + (" " * SELLER_WIDTH) + ZERO_PRICE

    @classmethod
    def _parse(cls, line, path, line_number):
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

    @classmethod
    def _format(cls, game, path, line_number):
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

    @classmethod
    def _is_end(cls, line):
        return cls._end_line(line, GAME_NAME_WIDTH, " " * OWNER_WIDTH)

    @classmethod
    def _end_record(cls):
        return END_MARKER.ljust(COLLECTION_WIDTH)

    @classmethod
    def _parse(cls, line, path, line_number):
        game_name = cls._text_field(
            line[:GAME_NAME_WIDTH], path, line_number, "game name", MAX_GAME_NAME_LENGTH
        )
        owner = cls._text_field(
            line[GAME_NAME_WIDTH:], path, line_number, "owner", MAX_USERNAME_LENGTH
        )
        return OwnedGame(game_name, owner)

    @classmethod
    def _format(cls, owned_game, path, line_number):
        game_name = cls._format_text(
            owned_game.game_name, path, line_number, "game name",
            GAME_NAME_WIDTH, MAX_GAME_NAME_LENGTH
        )
        owner = cls._format_text(
            owned_game.owner, path, line_number, "owner", OWNER_WIDTH, MAX_USERNAME_LENGTH
        )
        return game_name + owner
