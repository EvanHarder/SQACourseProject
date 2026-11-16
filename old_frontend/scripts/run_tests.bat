@echo off
REM run_tests.bat - Windows version of run_tests.sh
REM Usage:   scripts\run_tests.bat <path-to-frontend.exe>
REM Layout:  TestCases\<feature>\<success|failure>\<test>\input.txt, expected_output.txt, daily_transaction.txt
REM Front End command line: frontend.exe <accounts> <games> <collection> <daily_transaction_file>
REM Same steps as run_tests.sh: run each test, save both outputs, compare them with fc,
REM and write PASS/FAIL to results\run_<date>_<time>\report.txt
setlocal enabledelayedexpansion

REM --- 1. Check the argument: the path to frontend.exe ---

if "%~1"=="" (
    echo Usage: %0 ^<path-to-frontend.exe^>
    exit /b 1
)
REM full path to the Front End, so it works from any folder
set FE=%~f1
REM --- 2. Find the test cases and the shared data files ---
set ROOT=%~dp0..
set TESTS=%ROOT%\TestCases
set DATA=%TESTS%\data
if not exist "%DATA%\current_user_accounts.txt" (
    echo Cannot find %DATA%\current_user_accounts.txt
    exit /b 1
)
REM --- 3. Create a new results folder for this run, named from the date and time ---
REM (the order of the date parts depends on the Windows region setting)
for /f "tokens=1-3 delims=/:. " %%a in ("%time: =0%") do set T=%%a%%b%%c
for /f "tokens=1-3 delims=/-. " %%a in ("%date:~-10%") do set D=%%c%%a%%b
set RUN=%ROOT%\results\run_%D%_%T%
set REPORT=%RUN%\report.txt
set PASS=0
set FAIL=0
mkdir "%RUN%"
echo Front End requirements test run: run_%D%_%T% > "%REPORT%"

REM --- 4. Run every test: feature\outcome\test folders, skipping TestCases\data ---
for /d %%F in ("%TESTS%\*") do if /i not "%%~nxF"=="data" (
    for /d %%O in ("%%F\*") do (
        for /d %%T in ("%%O\*") do (
            set REL=%%~nxF\%%~nxO\%%~nxT
            set OUT_DIR=%RUN%\!REL!
            mkdir "!OUT_DIR!"
            REM run the Front End: input.txt is standard input; save the screen output and the daily transaction file
            "%FE%" "%DATA%\current_user_accounts.txt" "%DATA%\available_games.txt" "%DATA%\game_collection.txt" "!OUT_DIR!\actual_daily_transaction.txt" < "%%T\input.txt" > "!OUT_DIR!\actual_output.txt" 2>&1
            REM an empty expected daily_transaction.txt means "no file written"
            if not exist "!OUT_DIR!\actual_daily_transaction.txt" type nul > "!OUT_DIR!\actual_daily_transaction.txt"
            REM compare both outputs with the expected files; any difference fails the test
            set OK=1
            fc "%%T\expected_output.txt" "!OUT_DIR!\actual_output.txt" > "!OUT_DIR!\diff.txt" || set OK=0
            fc "%%T\daily_transaction.txt" "!OUT_DIR!\actual_daily_transaction.txt" >> "!OUT_DIR!\diff.txt" || set OK=0
            REM record the result; diff.txt is kept only for failing tests
            if !OK!==1 (
                set /a PASS+=1
                echo PASS  !REL! >> "%REPORT%"
                del "!OUT_DIR!\diff.txt"
            ) else (
                set /a FAIL+=1
                echo FAIL  !REL! >> "%REPORT%"
            )
        )
    )
)
REM --- 5. Summary: totals at the end of the report, then print the report ---
echo Passed: %PASS%   Failed: %FAIL% >> "%REPORT%"
type "%REPORT%"
endlocal
