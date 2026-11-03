#!/bin/bash
# check_outputs.sh - re-checks the actual outputs of an earlier test run against
# the expected outputs, without running the Front End again.
#
# Usage:   ./scripts/check_outputs.sh [run folder]
# Example: ./scripts/check_outputs.sh                          (latest run)
#          ./scripts/check_outputs.sh results/run_20261102_2215
#
# For every test in the run folder it compares:
#   actual_output.txt             with TestCases/<test>/expected_output.txt    (terminal log)
#   actual_daily_transaction.txt  with TestCases/<test>/daily_transaction.txt  (daily transaction file)
# and prints PASS or FAIL, followed by the differences for each failure.

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TESTS="$ROOT/TestCases"
RUN="${1:-$ROOT/results/latest}"

if [ ! -d "$RUN" ]; then
    echo "No run folder found at $RUN (run ./scripts/run_tests.sh first)"
    exit 1
fi

PASS=0
FAIL=0
for ACTUAL_OUTPUT in "$RUN"/*/*/*/actual_output.txt; do
    TEST_OUT_DIR="$(dirname "$ACTUAL_OUTPUT")"
    TEST_NAME="${TEST_OUT_DIR#$RUN/}"                 # e.g. buy/success/buy01
    EXPECTED_DIR="$TESTS/$TEST_NAME"

    TERMINAL_DIFF="$(diff "$EXPECTED_DIR/expected_output.txt" "$ACTUAL_OUTPUT")"
    FILE_DIFF="$(diff "$EXPECTED_DIR/daily_transaction.txt" "$TEST_OUT_DIR/actual_daily_transaction.txt")"

    if [ -z "$TERMINAL_DIFF" ] && [ -z "$FILE_DIFF" ]; then
        PASS=$((PASS + 1))
        echo "PASS  $TEST_NAME"
    else
        FAIL=$((FAIL + 1))
        echo "FAIL  $TEST_NAME"
        [ -n "$TERMINAL_DIFF" ] && printf '  terminal output (< expected, > actual):\n%s\n' "$(echo "$TERMINAL_DIFF" | sed 's/^/    /')"
        [ -n "$FILE_DIFF" ] && printf '  daily transaction file (< expected, > actual):\n%s\n' "$(echo "$FILE_DIFF" | sed 's/^/    /')"
    fi
done

echo "----------------------------------------"
echo "Passed: $PASS   Failed: $FAIL   Total: $((PASS + FAIL))"
[ $FAIL -eq 0 ]
