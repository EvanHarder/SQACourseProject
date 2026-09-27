#!/bin/bash
# run_tests.sh - runs every Front End requirements test and compares results
#
# Usage:   ./scripts/run_tests.sh <path-to-frontend-executable> [feature]
# Example: ./scripts/run_tests.sh ./frontend            (all features)
#          ./scripts/run_tests.sh ./frontend buy        (only tests/buy)
#
# Test layout:  tests/<feature>/<success|failure>/<test>/
#                   input.txt              transaction stream (standard input)
#                   expected_output.txt    expected terminal output
#                   daily_transaction.txt  expected daily transaction file (empty = no file written)
#
# Front End command line assumed by these tests:
#   frontend <current_user_accounts> <available_games> <game_collection> <daily_transaction_file>

FE="$1"
ONLY="$2"
if [ -z "$FE" ] || [ ! -x "$FE" ]; then
    echo "Usage: $0 <path-to-frontend-executable> [feature]"
    exit 1
fi
FE="$(cd "$(dirname "$FE")" && pwd)/$(basename "$FE")"

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DATA="$ROOT/data"
TESTS="$ROOT/tests"
RUN_ID="run_$(date +%Y%m%d_%H%M%S)"
RUN="$ROOT/results/$RUN_ID"
REPORT="$RUN/report.txt"

mkdir -p "$RUN"
PASS=0
FAIL=0

{
    echo "Front End requirements test run: $RUN_ID"
    echo "Executable: $FE"
    echo "----------------------------------------"
} > "$REPORT"

for TEST_DIR in "$TESTS"/*/*/*/; do
    REL="${TEST_DIR#$TESTS/}"; REL="${REL%/}"          # e.g. create/failure/create10
    FEATURE="${REL%%/*}"
    if [ -n "$ONLY" ] && [ "$FEATURE" != "$ONLY" ]; then
        continue
    fi
    OUT_DIR="$RUN/$REL"
    mkdir -p "$OUT_DIR"
    OUT="$OUT_DIR/actual_output.txt"
    ATF="$OUT_DIR/actual_daily_transaction.txt"
    DIFF="$OUT_DIR/diff.txt"

    "$FE" "$DATA/current_user_accounts.txt" "$DATA/available_games.txt" \
          "$DATA/game_collection.txt" "$ATF" < "$TEST_DIR/input.txt" > "$OUT" 2>&1
    # an empty expected daily_transaction.txt means "no file written"
    [ -f "$ATF" ] || touch "$ATF"

    OK=1
    echo "--- terminal output ---" > "$DIFF"
    diff "$TEST_DIR/expected_output.txt" "$OUT" >> "$DIFF" || OK=0
    echo "--- daily transaction file ---" >> "$DIFF"
    diff "$TEST_DIR/daily_transaction.txt" "$ATF" >> "$DIFF" || OK=0

    if [ $OK -eq 1 ]; then
        PASS=$((PASS + 1)); echo "PASS  $REL" >> "$REPORT"; rm -f "$DIFF"
    else
        FAIL=$((FAIL + 1)); echo "FAIL  $REL  (see $REL/diff.txt)" >> "$REPORT"
    fi
done

{
    echo "----------------------------------------"
    echo "Passed: $PASS   Failed: $FAIL   Total: $((PASS + FAIL))"
} >> "$REPORT"

cat "$REPORT"
ln -sfn "$RUN_ID" "$ROOT/results/latest"
[ $FAIL -eq 0 ]
