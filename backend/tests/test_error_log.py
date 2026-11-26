# test_error_log.py
#
# Statement coverage for error_log.py (ErrorLog.constraint and ErrorLog.fatal).
# Test IDs EL-01 and EL-02 match the test report.

import pytest

from error_log import ErrorLog


class TestErrorLog:

    # EL-01: a constraint error is printed and the Back End keeps going
    def test_constraint_prints_and_continues(self, capsys):
        ErrorLog.constraint("user does not exist.")
        assert capsys.readouterr().out == "ERROR: user does not exist.\n"

    # EL-02: a fatal error names the file and stops with exit code 1
    def test_fatal_prints_and_exits(self, capsys):
        with pytest.raises(SystemExit) as stopped:
            ErrorLog.fatal("f.txt", "bad line")
        assert stopped.value.code == 1
        assert capsys.readouterr().out == "ERROR: f.txt: bad line\n"
