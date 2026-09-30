@echo off
REM run_tests.bat - Windows version of run_tests.sh
REM Usage:   scripts\run_tests.bat <path-to-frontend.exe>
REM Layout:  TestCases\<feature>\<success|failure>\<test>\input.txt, expected_output.txt, daily_transaction.txt
REM Front End command line: frontend.exe <accounts> <games> <collection> <daily_transaction_file>
setlocal enabledelayedexpansion

if "%~1"=="" (
    echo Usage: %0 ^<path-to-frontend.exe^>
    exit /b 1
)
set FE=%~f1
set ROOT=%~dp0..
set TESTS=%ROOT%\TestCases
set DATA=%TESTS%\data
if not exist "%DATA%\current_user_accounts.txt" (
    echo Cannot find %DATA%\current_user_accounts.txt
    exit /b 1
)
for /f "tokens=1-3 delims=/:. " %%a in ("%time: =0%") do set T=%%a%%b%%c
for /f "tokens=1-3 delims=/-. " %%a in ("%date:~-10%") do set D=%%c%%a%%b
set RUN=%ROOT%\results\run_%D%_%T%
set REPORT=%RUN%\report.txt
set PASS=0
set FAIL=0
mkdir "%RUN%"
echo Front End requirements test run: run_%D%_%T% > "%REPORT%"

for /d %%F in ("%TESTS%\*") do if /i not "%%~nxF"=="data" (
    for /d %%O in ("%%F\*") do (
        for /d %%T in ("%%O\*") do (
            set REL=%%~nxF\%%~nxO\%%~nxT
            set OUT_DIR=%RUN%\!REL!
            mkdir "!OUT_DIR!"
            "%FE%" "%DATA%\current_user_accounts.txt" "%DATA%\available_games.txt" "%DATA%\game_collection.txt" "!OUT_DIR!\actual_daily_transaction.txt" < "%%T\input.txt" > "!OUT_DIR!\actual_output.txt" 2>&1
            if not exist "!OUT_DIR!\actual_daily_transaction.txt" type nul > "!OUT_DIR!\actual_daily_transaction.txt"
            set OK=1
            fc "%%T\expected_output.txt" "!OUT_DIR!\actual_output.txt" > "!OUT_DIR!\diff.txt" || set OK=0
            fc "%%T\daily_transaction.txt" "!OUT_DIR!\actual_daily_transaction.txt" >> "!OUT_DIR!\diff.txt" || set OK=0
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
echo Passed: %PASS%   Failed: %FAIL% >> "%REPORT%"
type "%REPORT%"
endlocal
