#!/bin/bash
# check_outputs.sh - Phase 3, step 6: compare a test run's actual outputs with the expected ones.
#
# Usage (from anywhere):
#   ./check_outputs.sh                          check the most recent run in outputs/
#   ./check_outputs.sh outputs/2026-11-05_150000  check a particular run
#
# run_tests.sh calls this automatically after every run, but it can also be
# run on its own to re-check an earlier run.
#
# For every test input in tests/inputs/ it makes two comparisons:
#   1. transaction file:  <run>/<test>.atf  against  tests/expected/<test>.etf
#   2. terminal output:   <run>/<test>.out  against  tests/expected/<test>.out
# A missing newline at the very end of a file is ignored; every other
# difference (spacing, padding, wording, line order) counts as a failure.
#
# Results are written into the run folder:
#   report.txt    one line per test: PASS, or FAIL and which output was wrong
#   failures.txt  the full diff for every failure (used to fill in the failure log)
# Exits with 0 if every test passed and 1 if any failed.

# Work from the folder this script is in, so it can be run from anywhere
cd "$(dirname "$0")" || exit 1

INPUT_DIR="tests/inputs"
EXPECTED_DIR="tests/expected"
OUTPUT_ROOT="outputs"

# ---- Which run to check: the one given, or the newest folder in outputs/ ----
run_dir="${1#phase3/}"
if [ -z "$run_dir" ]; then
    run_dir=$(ls -d "$OUTPUT_ROOT"/*/ 2>/dev/null | sort | tail -n 1)
    run_dir="${run_dir%/}"
fi
if [ ! -d "$run_dir" ]; then
    echo "No test run found to check. Run ./run_tests.sh first."
    exit 1
fi

report="$run_dir/report.txt"
failures="$run_dir/failures.txt"
: > "$report"
: > "$failures"

# Prints the differences between an expected file and an actual file
# (with "<" lines expected and ">" lines actual), ignoring a missing final newline.
# Returns 0 if they match.
compare() {
    local expected_file="$1"
    local actual_file="$2"
    diff <(sed -e '$a\' "$expected_file") <(sed -e '$a\' "$actual_file")
}

passed=0
failed=0

for input_file in "$INPUT_DIR"/*.inp; do
    test_name=$(basename "$input_file" .inp)

    # Only check tests that were part of this run (a run may cover just one command)
    [ -e "$run_dir/$test_name.out" ] || continue

    problems=()
    transaction_diff=$(compare "$EXPECTED_DIR/$test_name.etf" "$run_dir/$test_name.atf" 2>&1) \
        || problems+=("transaction file")
    terminal_diff=$(compare "$EXPECTED_DIR/$test_name.out" "$run_dir/$test_name.out" 2>&1) \
        || problems+=("terminal output")

    if [ ${#problems[@]} -eq 0 ]; then
        echo "$test_name: PASS" >> "$report"
        passed=$((passed + 1))
        continue
    fi

    # Record which outputs were wrong, and the full differences for the failure log
    summary=$(IFS=, ; echo "${problems[*]}" | sed 's/,/, /g')
    echo "$test_name: FAIL ($summary)" | tee -a "$report"
    failed=$((failed + 1))
    {
        echo "==================== $test_name: FAIL ($summary)"
        if [ -n "$transaction_diff" ]; then
            echo "--- transaction file (< expected $EXPECTED_DIR/$test_name.etf, > actual $test_name.atf)"
            echo "$transaction_diff"
        fi
        if [ -n "$terminal_diff" ]; then
            echo "--- terminal output (< expected $EXPECTED_DIR/$test_name.out, > actual $test_name.out)"
            echo "$terminal_diff"
        fi
        echo
    } >> "$failures"
done

echo "Total: $((passed + failed))  Passed: $passed  Failed: $failed" | tee -a "$report"
echo "Report: phase3/$report"
[ "$failed" -eq 0 ] && rm -f "$failures" || echo "Differences: phase3/$failures"

[ "$failed" -eq 0 ]
