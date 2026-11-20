#!/bin/bash
# run_tests.sh - Phase 3, step 5: run the Front End on every test input.
#
# Usage (from anywhere):
#   ./run_tests.sh              build the Front End, run every test, then check the results
#   ./run_tests.sh search       only run tests whose name starts with "search"
#
# Test layout (all paths are relative to this phase3/ folder):
#   tests/inputs/<test>.inp      what the user types in one test (fed to the Front End as stdin)
#   tests/expected/<test>.out    expected terminal output
#   tests/expected/<test>.etf    expected daily transaction file
#   tests/data/                  current user accounts, available games and game collection
#                                files that every test starts from
#   tests/data/<command>/        if this folder exists, tests named <command>_NN start from
#                                these files instead (e.g. tests/data/search/)
#
# Each run gets a new folder, so results from earlier runs are never overwritten:
#   outputs/<date>_<time>/<test>.out   actual terminal output (stdout and stderr)
#   outputs/<date>_<time>/<test>.atf   actual daily transaction file
#
# After running every test, this script calls check_outputs.sh on the new
# folder, which compares the actual files against the expected ones and writes
# report.txt and failures.txt there.

# Work from the folder this script is in, so it can be run from anywhere
cd "$(dirname "$0")" || exit 1

# ---- Settings ----
FRONTEND_DIR="../phase2"                        # where the Front End source and Makefile are
FRONTEND="$FRONTEND_DIR/frontend"               # the program being tested
INPUT_DIR="tests/inputs"
DEFAULT_DATA_DIR="tests/data"
OUTPUT_ROOT="outputs"
name_prefix="$1"                                # optional: only run tests starting with this

# ---- 1. Build the Front End so the tests always run the latest code ----
echo "Building the Front End..."
if ! make -s -C "$FRONTEND_DIR"; then
    echo "Build failed, no tests were run."
    exit 1
fi

# ---- 2. Make a new, empty folder for this run's actual outputs ----
run_dir="$OUTPUT_ROOT/$(date +%Y-%m-%d_%H%M%S)"
mkdir -p "$run_dir"

# ---- 3. Run every test input ----
test_count=0
for input_file in "$INPUT_DIR/$name_prefix"*.inp; do
    [ -e "$input_file" ] || { echo "No tests found matching $INPUT_DIR/$name_prefix*.inp"; exit 1; }

    test_name=$(basename "$input_file" .inp)    # e.g. sell_05
    command_name=${test_name%_*}                # e.g. sell

    # Use the command's own starting files if it has them, otherwise the shared ones
    data_dir="$DEFAULT_DATA_DIR"
    if [ -d "$DEFAULT_DATA_DIR/$command_name" ]; then
        data_dir="$DEFAULT_DATA_DIR/$command_name"
    fi

    actual_transactions="$run_dir/$test_name.atf"
    actual_terminal="$run_dir/$test_name.out"

    echo "running test $test_name"
    "$FRONTEND" "$data_dir/users.txt" "$data_dir/games.txt" "$data_dir/collection.txt" \
        "$actual_transactions" < "$input_file" > "$actual_terminal" 2>&1

    # The Front End only writes the transaction file at logout. A test with no
    # logout gets an empty file, so every test has both outputs to compare.
    [ -e "$actual_transactions" ] || : > "$actual_transactions"

    test_count=$((test_count + 1))
done

echo "Ran $test_count tests. Actual outputs saved in phase3/$run_dir/"
echo

# ---- 4. Compare this run's outputs against the expected outputs ----
./check_outputs.sh "$run_dir"
