@echo off
echo Starting automated tests...
set GAME_EXE=..\build\bin\Release\RichGame.exe
set TEST_DIR=.
set RESULT_FILE=%TEST_DIR%\checkresult.txt
set CASES_DIR=%TEST_DIR%\cases
set ARTIFACTS_DIR=%TEST_DIR%\artifacts

rem Clean previous results
if exist %RESULT_FILE% del %RESULT_FILE%
if exist %ARTIFACTS_DIR% rmdir /s /q %ARTIFACTS_DIR%
mkdir %ARTIFACTS_DIR% >nul 2>&1

echo Running hello world test with command list...

rem Run the test with command input from file
%GAME_EXE% < %CASES_DIR%\helloworld\cmdlist.txt > %ARTIFACTS_DIR%\helloworld_actual.txt

rem Compare with expected output
fc %ARTIFACTS_DIR%\helloworld_actual.txt %CASES_DIR%\helloworld\expected.txt > nul
if %errorlevel% equ 0 (
    echo case1 helloworld result PASS >> %RESULT_FILE%
    echo PASS: Hello world test with cmdlist
) else (
    echo case1 helloworld result FAIL >> %RESULT_FILE%
    echo FAIL: Hello world test with cmdlist
    echo Expected:
    type %TEST_DIR%\expected_helloworld.txt
    echo.
    echo Actual:
    type %TEST_DIR%\actual_output.txt
)

echo.
echo Test results saved to %RESULT_FILE%
type %RESULT_FILE%