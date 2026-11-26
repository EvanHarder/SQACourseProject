# test_transaction_file.py
#
# Statement coverage for every method in transaction_file.py (Transaction,
# TransactionFileReader, _FieldReader), plus full path coverage for
# TransactionFileReader.read.
#
# Test IDs (TF-01 to TF-30, PC-01 to PC-18) match the test report.
#
# Expected results come from the requirements and the Phase 4 design, not from
# what the code currently does, so a failing test points at a real defect.

import pytest

from transaction_file import Transaction, TransactionFileReader, _FieldReader

# ---- Real transaction lines, in the format Front End A writes ----
CREATE_LINE = "01 newuser         FS 000000.00"
DELETE_LINE = "02 buyuser         BS 000500.00"
ADDCREDIT_LINE = "06 fulluser        FS 000100.00"
END_LINE = "00                    000000.00"
SELL_LINE = "03 Celeste                    fulluser        020.00"
BUY_LINE = "04 Stardew Valley             selluser        buyuser         015.00"
REFUND_LINE = "05 buyuser         selluser        000015.00"
BAD_LINE = "07 newuser         FS 000000.00"   # unknown transaction code


# Writes the given lines to a file in pytest's temporary folder and returns its path.
def write_file(tmp_path, lines, name="daily.txt"):
    path = tmp_path / name
    path.write_text("".join(line + "\n" for line in lines))
    return str(path)


# Runs code that should stop the Back End, and returns what it printed.
def expect_fatal(capsys, action):
    with pytest.raises(SystemExit) as stopped:
        action()
    assert stopped.value.code == 1
    return capsys.readouterr().out


class TestTransaction:

    # TF-01: the constructor stores the fields given and leaves the rest empty
    def test_constructor_stores_fields(self):
        t = Transaction("01", username="newuser", user_type="FS", credit_cents=500)
        assert (t.code, t.username, t.user_type, t.credit_cents) == ("01", "newuser", "FS", 500)
        assert (t.game_name, t.seller, t.buyer, t.price_cents) == ("", "", "", 0)

    # TF-02: sell description
    def test_describe_sell(self):
        t = Transaction("03", game_name="Hades", seller="selluser", price_cents=2500)
        assert t.describe() == "sell Hades by selluser"

    # TF-03: buy description
    def test_describe_buy(self):
        t = Transaction("04", game_name="Hades", seller="selluser", buyer="buyuser")
        assert t.describe() == "buy Hades from selluser by buyuser"

    # TF-04: refund description
    def test_describe_refund(self):
        t = Transaction("05", seller="selluser", buyer="buyuser")
        assert t.describe() == "refund from selluser to buyuser"

    # TF-05: account transactions use their name from the lookup table
    def test_describe_create(self):
        assert Transaction("01", username="newuser").describe() == "create newuser"

    # TF-06: an unknown code falls back to the code itself
    def test_describe_unknown_code(self):
        assert Transaction("09", username="newuser").describe() == "09 newuser"


class TestTransactionFileReaderRead:
    # Statement coverage for read (full path coverage is in TestReadPathCoverage)

    # TF-07: a file that doesn't exist is a fatal error
    def test_read_missing_file(self, tmp_path, capsys):
        missing = str(tmp_path / "missing.txt")
        out = expect_fatal(capsys, lambda: TransactionFileReader.read(missing))
        assert out == "ERROR: " + missing + ": cannot open file\n"

    # TF-08: blank lines and 00 lines are skipped, other transactions are kept
    def test_read_skips_blank_and_end_lines(self, tmp_path):
        path = write_file(tmp_path, ["", END_LINE, CREATE_LINE])
        transactions = TransactionFileReader.read(path)
        assert [t.code for t in transactions] == ["01"]

    # TF-09: a bad line inside the file is fatal and names its line number
    def test_read_bad_line(self, tmp_path, capsys):
        path = write_file(tmp_path, [BAD_LINE])
        out = expect_fatal(capsys, lambda: TransactionFileReader.read(path))
        assert out == "ERROR: " + path + ": line 1: unknown transaction code '07'\n"


class TestParseLine:

    # TF-10: create, delete and addcredit lines share the account format
    @pytest.mark.parametrize("line, code, username, user_type, credit", [
        (CREATE_LINE, "01", "newuser", "FS", 0),
        (DELETE_LINE, "02", "buyuser", "BS", 50000),
        (ADDCREDIT_LINE, "06", "fulluser", "FS", 10000),
    ])
    def test_parse_account_lines(self, line, code, username, user_type, credit):
        t = TransactionFileReader.parse_line(line, "daily.txt", 1)
        assert (t.code, t.username, t.user_type, t.credit_cents) == (code, username, user_type, credit)

    # TF-11: the 00 line is allowed to have a blank username and type
    def test_parse_end_of_session(self):
        t = TransactionFileReader.parse_line(END_LINE, "daily.txt", 1)
        assert (t.code, t.username, t.user_type, t.credit_cents) == ("00", "", "", 0)

    # TF-12: sell line
    def test_parse_sell(self):
        t = TransactionFileReader.parse_line(SELL_LINE, "daily.txt", 1)
        assert (t.code, t.game_name, t.seller, t.price_cents) == ("03", "Celeste", "fulluser", 2000)

    # TF-13: buy line (the buyer is also stored as the username)
    def test_parse_buy(self):
        t = TransactionFileReader.parse_line(BUY_LINE, "daily.txt", 1)
        assert (t.code, t.game_name, t.seller, t.buyer, t.price_cents) == ("04", "Stardew Valley", "selluser", "buyuser", 1500)
        assert t.username == "buyuser"

    # TF-14: refund line (buyer first, then seller)
    def test_parse_refund(self):
        t = TransactionFileReader.parse_line(REFUND_LINE, "daily.txt", 1)
        assert (t.code, t.buyer, t.seller, t.credit_cents) == ("05", "buyuser", "selluser", 1500)

    # TF-15: an unknown code is fatal
    def test_parse_unknown_code(self, capsys):
        out = expect_fatal(capsys, lambda: TransactionFileReader.parse_line(BAD_LINE, "daily.txt", 3))
        assert out == "ERROR: daily.txt: line 3: unknown transaction code '07'\n"

    # TF-16: a user type other than AA, FS, BS, SS or AM is fatal
    def test_parse_invalid_user_type(self, capsys):
        line = "01 newuser         ZZ 000000.00"
        out = expect_fatal(capsys, lambda: TransactionFileReader.parse_line(line, "daily.txt", 1))
        assert out == "ERROR: daily.txt: line 1: bad user type 'ZZ'\n"


class TestFieldReader:

    # TF-17: the constructor stores the line details and starts at position 0
    def test_constructor(self):
        reader = _FieldReader("01 abc", "daily.txt", 4)
        assert (reader.line, reader.path, reader.line_number, reader.position) == ("01 abc", "daily.txt", 4, 0)

    # TF-18: fail names the file and line, then stops
    def test_fail(self, capsys):
        reader = _FieldReader("01", "daily.txt", 4)
        out = expect_fatal(capsys, lambda: reader.fail("x"))
        assert out == "ERROR: daily.txt: line 4: x\n"

    # TF-19: take returns the next characters and moves along the line
    def test_take(self):
        reader = _FieldReader("01 abc", "daily.txt", 1)
        assert reader.take(2) == "01"
        assert reader.position == 2

    # TF-20: taking past the end of the line is fatal
    def test_take_past_end(self, capsys):
        reader = _FieldReader("01", "daily.txt", 1)
        out = expect_fatal(capsys, lambda: reader.take(5))
        assert out == "ERROR: daily.txt: line 1: line is too short\n"

    # TF-21: a single space between fields is accepted
    def test_take_separator(self):
        reader = _FieldReader(" x", "daily.txt", 1)
        reader.take_separator()
        assert reader.position == 1

    # TF-22: anything other than a space between fields is fatal
    def test_take_separator_not_space(self, capsys):
        reader = _FieldReader("_x", "daily.txt", 1)
        out = expect_fatal(capsys, reader.take_separator)
        assert out == "ERROR: daily.txt: line 1: fields must be separated by one space\n"

    # TF-23: a text field comes back without its padding
    def test_take_text(self):
        reader = _FieldReader(" newuser        ", "daily.txt", 1)
        assert reader.take_text(15) == "newuser"

    # TF-24: a blank required field is fatal
    def test_take_text_blank_not_allowed(self, capsys):
        reader = _FieldReader(" " * 16, "daily.txt", 1)
        out = expect_fatal(capsys, lambda: reader.take_text(15))
        assert out == "ERROR: daily.txt: line 1: a required field is blank\n"

    # TF-25: a blank field is fine where it is allowed (the 00 line)
    def test_take_text_blank_allowed(self):
        reader = _FieldReader(" " * 16, "daily.txt", 1)
        assert reader.take_text(15, allow_blank=True) == ""

    # TF-26: a money field becomes whole cents
    def test_take_money(self):
        reader = _FieldReader(" 000100.00", "daily.txt", 1)
        assert reader.take_money(9) == 10000

    # TF-27: the decimal point in the wrong place is fatal
    def test_take_money_bad_decimal(self, capsys):
        reader = _FieldReader(" 0001000.0", "daily.txt", 1)
        out = expect_fatal(capsys, lambda: reader.take_money(9))
        assert out == "ERROR: daily.txt: line 1: bad money amount '0001000.0'\n"

    # TF-28: a letter in the amount is fatal
    def test_take_money_not_digits(self, capsys):
        reader = _FieldReader(" 00010x.00", "daily.txt", 1)
        out = expect_fatal(capsys, lambda: reader.take_money(9))
        assert out == "ERROR: daily.txt: line 1: bad money amount '00010x.00'\n"

    # TF-29: finish passes when the whole line has been read
    def test_finish_at_end(self):
        reader = _FieldReader("01", "daily.txt", 1)
        reader.take(2)
        reader.finish()

    # TF-30: anything left after the last field is fatal
    def test_finish_extra_characters(self, capsys):
        reader = _FieldReader("01 extra", "daily.txt", 1)
        reader.take(2)
        out = expect_fatal(capsys, reader.finish)
        assert out == "ERROR: daily.txt: line 1: line is too long\n"


class TestReadPathCoverage:
    # PATH COVERAGE: TransactionFileReader.read
    #
    # Decisions in read:
    #   D1  the file opens, or OSError gives a fatal error
    #   L   the loop over lines runs 0, 1 or 2 times
    # Each loop iteration takes one of four body paths:
    #   B  blank line, skipped            X  bad line, fatal (ends the loop)
    #   E  00 line, skipped               T  transaction, appended
    # X can only be the last iteration, because a fatal error stops the program.

    BODY = {"B": "", "X": BAD_LINE, "E": END_LINE, "T": CREATE_LINE}

    # PC-01: D1 fails
    def test_pc01_missing_file(self, tmp_path, capsys):
        missing = str(tmp_path / "missing.txt")
        out = expect_fatal(capsys, lambda: TransactionFileReader.read(missing))
        assert out == "ERROR: " + missing + ": cannot open file\n"

    # PC-02: loop runs 0 times
    def test_pc02_empty_file(self, tmp_path):
        assert TransactionFileReader.read(write_file(tmp_path, [])) == []

    # PC-03 to PC-06: loop runs once, through each body path
    # PC-07 to PC-18: loop runs twice, through every possible pair of body paths
    @pytest.mark.parametrize("path_id, paths, expected_codes", [
        ("PC-03", "B", []),
        ("PC-04", "X", None),
        ("PC-05", "E", []),
        ("PC-06", "T", ["01"]),
        ("PC-07", "BB", []),
        ("PC-08", "BX", None),
        ("PC-09", "BE", []),
        ("PC-10", "BT", ["01"]),
        ("PC-11", "EB", []),
        ("PC-12", "EX", None),
        ("PC-13", "EE", []),
        ("PC-14", "ET", ["01"]),
        ("PC-15", "TB", ["01"]),
        ("PC-16", "TX", None),
        ("PC-17", "TE", ["01"]),
        ("PC-18", "TT", ["01", "06"]),
    ])
    def test_loop_paths(self, tmp_path, capsys, path_id, paths, expected_codes):
        lines = [self.BODY[p] for p in paths]
        if paths == "TT":
            lines[1] = ADDCREDIT_LINE  # two different transactions, to check the order is kept
        path = write_file(tmp_path, lines)

        if expected_codes is None:
            # The bad line is always the last one, so the error names that line
            out = expect_fatal(capsys, lambda: TransactionFileReader.read(path))
            assert out == "ERROR: " + path + ": line " + str(len(lines)) + ": unknown transaction code '07'\n"
        else:
            assert [t.code for t in TransactionFileReader.read(path)] == expected_codes
