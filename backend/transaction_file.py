# transaction_file.py
#
# Reads the merged daily transaction file written by the Front End. Each line
# is one transaction, and every Front End session ends with a 00 line:
#
#   01/02/06/00  XX UUUUUUUUUUUUUUU TT CCCCCCCCC
#   03           XX IIIIIIIIIIIIIIIIIIIIIIIIII SSSSSSSSSSSSSSS PPPPPP
#   04           XX IIIIIIIIIIIIIIIIIIIIIIIIII SSSSSSSSSSSSSSS UUUUUUUUUUUUUUU PPPPPP
#   05           XX UUUUUUUUUUUUUUU SSSSSSSSSSSSSSS CCCCCCCCC
#
# XX code, U username (buyer for 04 and 05), T user type, C credit (000000.00),
# I game name, S seller, P price (000.00). Fields are padded with spaces and
# separated by one space.
#
# Classes:
#   Transaction            one line of the file, split into its fields
#   TransactionFileReader  reads the whole file into a list of Transaction

from error_log import ErrorLog
from data_files import VALID_USER_TYPES

# Transaction codes
CODE_END_OF_SESSION = "00"
CODE_CREATE = "01"
CODE_DELETE = "02"
CODE_SELL = "03"
CODE_BUY = "04"
CODE_REFUND = "05"
CODE_ADDCREDIT = "06"

# Field widths
USERNAME_WIDTH = 15
USER_TYPE_WIDTH = 2
CREDIT_WIDTH = 9
GAME_NAME_WIDTH = 26  # Front End A uses 26, as in the spec's examples
PRICE_WIDTH = 6


class Transaction:

    # Stores one transaction. Fields a code does not use are "" (text) or 0 (money, in cents).
    def __init__(self, code, username="", user_type="", credit_cents=0, game_name="", seller="", buyer="", price_cents=0):
        self.code = code
        self.username = username
        self.user_type = user_type
        self.credit_cents = credit_cents
        self.game_name = game_name
        self.seller = seller
        self.buyer = buyer
        self.price_cents = price_cents

    # Returns a short description of the transaction, used in error messages.
    def describe(self):
        if self.code == CODE_SELL:
            return "sell " + self.game_name + " by " + self.seller
        if self.code == CODE_BUY:
            return "buy " + self.game_name + " from " + self.seller + " by " + self.buyer
        if self.code == CODE_REFUND:
            return "refund from " + self.seller + " to " + self.buyer
        names = {CODE_CREATE: "create", CODE_DELETE: "delete", CODE_ADDCREDIT: "addcredit", CODE_END_OF_SESSION: "end of session"}
        return names.get(self.code, self.code) + " " + self.username


class TransactionFileReader:

    # Reads every transaction in the file, skipping the 00 end-of-session lines.
    # A missing file or a badly formatted line is a fatal error.
    @staticmethod
    def read(path):
        try:
            with open(path, "r") as file:
                lines = file.read().splitlines()
        except OSError:
            ErrorLog.fatal(path, "cannot open file")

        transactions = []
        for line_number, line in enumerate(lines, start=1):
            if line.strip() == "":
                continue  # ignore blank lines, such as at the end of the file
            transaction = TransactionFileReader.parse_line(line, path, line_number)
            if transaction.code != CODE_END_OF_SESSION:
                transactions.append(transaction)
        return transactions

    # Splits one line into a Transaction, or stops the Back End if the line is badly formatted.
    @staticmethod
    def parse_line(line, path, line_number):
        fields = _FieldReader(line, path, line_number)
        code = fields.take(2)

        if code in (CODE_CREATE, CODE_DELETE, CODE_ADDCREDIT, CODE_END_OF_SESSION):
            username = fields.take_text(USERNAME_WIDTH, allow_blank=(code == CODE_END_OF_SESSION))
            user_type = fields.take_text(USER_TYPE_WIDTH, allow_blank=(code == CODE_END_OF_SESSION))
            # Phase 5 fix (TF-16): reject unknown user types here, so a bad 01, 02 or 06
            # line is reported on its own line instead of crashing when the new files are written
            if code != CODE_END_OF_SESSION and user_type not in VALID_USER_TYPES:
                fields.fail("bad user type '" + user_type + "'")
            credit = fields.take_money(CREDIT_WIDTH)
            fields.finish()
            return Transaction(code, username=username, user_type=user_type, credit_cents=credit)

        if code == CODE_SELL:
            game_name = fields.take_text(GAME_NAME_WIDTH)
            seller = fields.take_text(USERNAME_WIDTH)
            price = fields.take_money(PRICE_WIDTH)
            fields.finish()
            return Transaction(code, game_name=game_name, seller=seller, price_cents=price)

        if code == CODE_BUY:
            game_name = fields.take_text(GAME_NAME_WIDTH)
            seller = fields.take_text(USERNAME_WIDTH)
            buyer = fields.take_text(USERNAME_WIDTH)
            price = fields.take_money(PRICE_WIDTH)
            fields.finish()
            return Transaction(code, username=buyer, game_name=game_name, seller=seller, buyer=buyer, price_cents=price)

        if code == CODE_REFUND:
            buyer = fields.take_text(USERNAME_WIDTH)
            seller = fields.take_text(USERNAME_WIDTH)
            credit = fields.take_money(CREDIT_WIDTH)
            fields.finish()
            return Transaction(code, username=buyer, seller=seller, buyer=buyer, credit_cents=credit)

        ErrorLog.fatal(path, "line " + str(line_number) + ": unknown transaction code '" + code + "'")


class _FieldReader:
    # Helper for parse_line: walks along one line, taking one fixed-width field at a time.

    # Starts at the beginning of the line.
    def __init__(self, line, path, line_number):
        self.line = line
        self.path = path
        self.line_number = line_number
        self.position = 0

    # Stops the Back End with a message that names the file and line.
    def fail(self, message):
        ErrorLog.fatal(self.path, "line " + str(self.line_number) + ": " + message)

    # Returns the next width characters exactly as they are (the code uses this).
    def take(self, width):
        end = self.position + width
        if end > len(self.line):
            self.fail("line is too short")
        text = self.line[self.position:end]
        self.position = end
        return text

    # Skips the single space that separates two fields.
    def take_separator(self):
        if self.take(1) != " ":
            self.fail("fields must be separated by one space")

    # Returns the next text field with its padding removed.
    def take_text(self, width, allow_blank=False):
        self.take_separator()
        text = self.take(width).rstrip(" ")
        if text == "" and not allow_blank:
            self.fail("a required field is blank")
        return text

    # Returns the next money field ("000100.00" or "015.00") as whole cents.
    def take_money(self, width):
        self.take_separator()
        text = self.take(width)
        dollars, dot, cents = text.partition(".")
        if dot != "." or len(cents) != 2 or not dollars.isdigit() or not cents.isdigit():
            self.fail("bad money amount '" + text + "'")
        return int(dollars) * 100 + int(cents)

    # Checks that nothing is left on the line after the last field.
    def finish(self):
        if self.position != len(self.line):
            self.fail("line is too long")
