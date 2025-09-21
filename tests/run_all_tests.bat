@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Rich Game Automated Testing Framework
echo ========================================

set GAME_EXE=..\build\bin\Release\RichGame.exe
set TEST_DIR=.
set RESULT_FILE=%TEST_DIR%\checkresult.txt
set CASES_DIR=%TEST_DIR%\cases
set ARTIFACTS_DIR=%TEST_DIR%\artifacts
set TOTAL_TESTS=0
set PASSED_TESTS=0

rem Clean previous results
if exist %RESULT_FILE% del %RESULT_FILE%
echo Test Results > %RESULT_FILE%
echo ============ >> %RESULT_FILE%
echo. >> %RESULT_FILE%

echo Checking if game executable exists...
if not exist %GAME_EXE% (
    echo ERROR: Game executable not found at %GAME_EXE%
    echo Please build the project first with: cmake --build . --config Release
    pause
    exit /b 1
)

echo Game executable found: %GAME_EXE%
echo.

rem Test Case 1: Hello World Test
echo Running Test Case 1: Hello World Test
echo --------------------------------------
set /a TOTAL_TESTS+=1

if exist %ARTIFACTS_DIR% rmdir /s /q %ARTIFACTS_DIR%
mkdir %ARTIFACTS_DIR% >nul 2>&1

%GAME_EXE% < %CASES_DIR%\helloworld\cmdlist.txt > %ARTIFACTS_DIR%\helloworld_actual.txt

fc %ARTIFACTS_DIR%\helloworld_actual.txt %CASES_DIR%\helloworld\expected.txt > nul
if !errorlevel! equ 0 (
    echo [PASS] Hello World Test
    echo case1 helloworld result PASS >> %RESULT_FILE%
    set /a PASSED_TESTS+=1
) else (
    echo [FAIL] Hello World Test
    echo case1 helloworld result FAIL >> %RESULT_FILE%
    echo Expected:
    type %TEST_DIR%\expected_helloworld.txt
    echo.
    echo Actual:
    type %TEST_DIR%\actual_helloworld.txt
    echo.
)

rem Test Case 2: Direct Command Test  
echo Running Test Case 2: Direct Command Test
echo ----------------------------------------
set /a TOTAL_TESTS+=1

if exist %ARTIFACTS_DIR%\direct_actual.txt del %ARTIFACTS_DIR%\direct_actual.txt

%GAME_EXE% testhelloworld > %ARTIFACTS_DIR%\direct_actual.txt
echo Hello World! > %ARTIFACTS_DIR%\expected_direct.txt

fc %ARTIFACTS_DIR%\direct_actual.txt %ARTIFACTS_DIR%\expected_direct.txt > nul
if !errorlevel! equ 0 (
    echo [PASS] Direct Command Test  
    echo case2 direct_command result PASS >> %RESULT_FILE%
    set /a PASSED_TESTS+=1
) else (
    echo [FAIL] Direct Command Test
    echo case2 direct_command result FAIL >> %RESULT_FILE%
)

rem Test Case 3: Invalid Command Test
echo Running Test Case 3: Invalid Command Test
echo -----------------------------------------
set /a TOTAL_TESTS+=1

if exist %ARTIFACTS_DIR%\invalid_actual.txt del %ARTIFACTS_DIR%\invalid_actual.txt

%GAME_EXE% invalidcommand > %ARTIFACTS_DIR%\invalid_actual.txt 2>&1

findstr /C:"Unknown command" %ARTIFACTS_DIR%\invalid_actual.txt > nul
if !errorlevel! equ 0 (
    echo [PASS] Invalid Command Test
    echo case3 invalid_command result PASS >> %RESULT_FILE%
    set /a PASSED_TESTS+=1
) else (
    echo [FAIL] Invalid Command Test
    echo case3 invalid_command result FAIL >> %RESULT_FILE%
)

echo.
echo ========================================
echo Test Summary
echo ========================================
echo Total Tests: !TOTAL_TESTS!
echo Passed:      !PASSED_TESTS!
echo Failed:      !SET /A FAILED=TOTAL_TESTS-PASSED_TESTS! & echo !FAILED!

if !PASSED_TESTS! equ !TOTAL_TESTS! (
    echo.
    echo ^>^>^> ALL TESTS PASSED! ^<^<^<
    echo Status: SUCCESS
) else (
    echo.
    echo ^>^>^> SOME TESTS FAILED ^<^<^<
    echo Status: FAILURE
)

echo.
echo >> %RESULT_FILE%
echo Test Summary: >> %RESULT_FILE%
echo Total: !TOTAL_TESTS!, Passed: !PASSED_TESTS!, Failed: !SET /A FAILED=TOTAL_TESTS-PASSED_TESTS! & echo !FAILED! >> %RESULT_FILE%

echo Detailed results saved to: %RESULT_FILE%
echo.
echo Press any key to view detailed results...
pause > nul
type %RESULT_FILE%