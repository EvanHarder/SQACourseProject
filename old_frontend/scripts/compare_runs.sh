#!/bin/bash
# compare_runs.sh - compares the results of two earlier test runs (regression check)
#
# Usage:   ./scripts/compare_runs.sh <older_run_id> <newer_run_id>
# Example: ./scripts/compare_runs.sh run_20261019_140000 run_20261106_101500
# Lists tests whose PASS/FAIL status changed and any output that changed between runs.

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OLD="$ROOT/results/$1"
NEW="$ROOT/results/$2"
if [ -z "$1" ] || [ -z "$2" ] || [ ! -d "$OLD" ] || [ ! -d "$NEW" ]; then
    echo "Usage: $0 <older_run_id> <newer_run_id>"
    echo "Available runs:"; ls "$ROOT/results" 2>/dev/null | grep '^run_'
    exit 1
fi

echo "Status changes ($1 -> $2):"
diff <(grep -E '^(PASS|FAIL)' "$OLD/report.txt" | awk '{print $2, $1}' | sort) \
     <(grep -E '^(PASS|FAIL)' "$NEW/report.txt" | awk '{print $2, $1}' | sort) \
     | grep -E '^[<>]' || echo "  none"

echo
echo "Output differences between runs:"
diff -r -q -x diff.txt "$OLD" "$NEW" | grep -v 'report.txt' || echo "  none"
