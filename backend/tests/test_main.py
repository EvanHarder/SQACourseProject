# test_main.py
#
# Statement coverage for main.py (BackEnd and main). Test IDs MN-01 to MN-05
# match the test report. These run the whole Back End on the sample files in
# backend/sample/, so they also check that the modules work together.

import os
import runpy
import sys

import pytest

import main
from main import BackEnd

SAMPLE_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "sample")
SAMPLE_INPUTS = [os.path.join(SAMPLE_DIR, name) for name in ("accounts.txt", "games.txt", "collection.txt", "merged_transactions.txt")]
EXPECTED_DIR = os.path.join(SAMPLE_DIR, "expected")
OUTPUT_NAMES = ("new_accounts.txt", "new_games.txt", "new_collection.txt")


# Returns the lines of a file without their line endings.
def read_lines(path):
    with open(path, "r") as file:
        return file.read().splitlines()


# Returns the three output paths inside pytest's temporary folder.
def output_paths(tmp_path):
    return [str(tmp_path / name) for name in OUTPUT_NAMES]


class TestBackEnd:

    # MN-01: the constructor stores the seven paths in command-line order
    def test_constructor_stores_paths(self):
        back_end = BackEnd(["a", "g", "c", "t", "na", "ng", "nc"])
        assert (back_end.accounts_path, back_end.games_path, back_end.collection_path, back_end.transactions_path) == ("a", "g", "c", "t")
        assert (back_end.new_accounts_path, back_end.new_games_path, back_end.new_collection_path) == ("na", "ng", "nc")

    # MN-02: running on the sample files gives the expected errors and new files
    def test_run_sample_files(self, tmp_path, capsys):
        outputs = output_paths(tmp_path)
        BackEnd(SAMPLE_INPUTS + outputs).run()
        assert capsys.readouterr().out == "ERROR: insufficient credit.\nERROR: username already exists.\n"
        for output, name in zip(outputs, OUTPUT_NAMES):
            # Compare line by line, so Windows (\r\n) and Linux (\n) line endings both pass
            assert read_lines(output) == read_lines(os.path.join(EXPECTED_DIR, name)), name + " does not match"

    # MN-03: a bad transaction file stops the Back End before any new file is written
    def test_run_bad_input_writes_nothing(self, tmp_path):
        bad = tmp_path / "bad.txt"
        bad.write_text("07 nobody          FS 000000.00\n")
        outputs = output_paths(tmp_path)
        with pytest.raises(SystemExit):
            BackEnd(SAMPLE_INPUTS[:3] + [str(bad)] + outputs).run()
        assert not any(os.path.exists(output) for output in outputs)


class TestMain:

    # MN-04: the wrong number of arguments prints the usage message and exits
    def test_main_wrong_argument_count(self, monkeypatch, capsys):
        monkeypatch.setattr(sys, "argv", ["main.py", "a", "b", "c"])
        with pytest.raises(SystemExit) as stopped:
            main.main()
        assert stopped.value.code == 1
        assert capsys.readouterr().out == main.USAGE + "\n"

    # MN-05: started as a script with seven arguments, it runs the Back End and writes the new files
    def test_main_runs_back_end(self, monkeypatch, tmp_path, capsys):
        outputs = output_paths(tmp_path)
        monkeypatch.setattr(sys, "argv", ["main.py"] + SAMPLE_INPUTS + outputs)
        runpy.run_path(main.__file__, run_name="__main__")  # same as "python main.py ..."
        assert all(os.path.exists(output) for output in outputs)
