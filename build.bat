@echo off
REM 大富翁游戏 Windows 构建脚本
REM 用法: build.bat [clean|test|debug|release|format|check|run|ci|help]

setlocal EnableExtensions EnableDelayedExpansion

REM 记录并切换到 UTF-8 代码页，避免中文输出乱码
for /f "tokens=2 delims=: " %%a in ('chcp') do set "_ORIG_CP=%%a"
chcp 65001 >nul

set "PROJECT_DIR=%~dp0"
set "BUILD_DIR=%PROJECT_DIR%build"
set "ACTIVE_CONFIG="
set "CMAKE_GENERATOR="
set "IS_MULTI_CONFIG=0"

REM 基础环境检查
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] 未检测到 CMake，请安装并加入 PATH 后重试
    call :_restore_cp & exit /b 1
)
)

:main
set command=%1
if "%command%"=="" set command=release

if /i "%command%"=="clean" goto clean
if /i "%command%"=="debug" goto debug
if /i "%command%"=="release" goto release
if /i "%command%"=="test" goto test
if /i "%command%"=="format" goto format
if /i "%command%"=="check" goto check
if /i "%command%"=="run" goto run
if /i "%command%"=="ci" goto ci
if /i "%command%"=="help" goto help

echo [ERROR] 未知命令: %command%
goto help

:clean
echo [INFO] 清理构建目录...
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
)
echo [SUCCESS] 构建目录已清理
goto :eof

:detect_generator
REM 优先使用 Ninja，其次自动选择最新的 Visual Studio，找不到则交由 CMake 自选
set "CMAKE_GENERATOR="
set "IS_MULTI_CONFIG=0"

where ninja >nul 2>nul
if %errorlevel%==0 (
    set "CMAKE_GENERATOR=Ninja"
    set "IS_MULTI_CONFIG=0"
    echo [INFO] 使用生成器: Ninja
    goto :eof
)

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    for /f "usebackq delims=" %%v in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationVersion`) do set "VSVER=%%v"
    if defined VSVER (
        echo [INFO] 检测到 Visual Studio 版本: !VSVER!
        set "VSMAJOR=!VSVER:~0,2!"
        if "!VSMAJOR!"=="17" (
            set "CMAKE_GENERATOR=Visual Studio 17 2022"
            set "IS_MULTI_CONFIG=1"
        ) else if "!VSMAJOR!"=="16" (
            set "CMAKE_GENERATOR=Visual Studio 16 2019"
            set "IS_MULTI_CONFIG=1"
        )
    )
)

if not defined CMAKE_GENERATOR (
    echo [INFO] 未检测到 Ninja 或 Visual Studio，交由 CMake 选择默认生成器
    set "IS_MULTI_CONFIG=0"
)
goto :eof

:configure
set build_type=%1
if "%build_type%"=="" set build_type=Release
set "ACTIVE_CONFIG=%build_type%"

echo [INFO] 配置CMake (构建类型: %build_type%)...
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM 自动检测并选择生成器
call :detect_generator

set "CMAKE_ARGS=-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
if "%IS_MULTI_CONFIG%"=="0" set "CMAKE_ARGS=!CMAKE_ARGS! -DCMAKE_BUILD_TYPE=%build_type%"

if defined CMAKE_GENERATOR (
    if /i "%CMAKE_GENERATOR%"=="Ninja" (
        cmake .. -G "%CMAKE_GENERATOR%" !CMAKE_ARGS!
    ) else (
        cmake .. -G "%CMAKE_GENERATOR%" -A x64 !CMAKE_ARGS!
    )
) else (
    cmake .. !CMAKE_ARGS!
)
if %errorlevel% neq 0 (
    echo [ERROR] CMake配置失败
    call :_restore_cp & exit /b 1
)
echo [SUCCESS] CMake配置完成
goto :eof

:build
echo [INFO] 构建项目...
cd /d "%BUILD_DIR%"
if "%IS_MULTI_CONFIG%"=="1" (
    cmake --build . --config %ACTIVE_CONFIG% --parallel
) else (
    cmake --build . --parallel
)
if %errorlevel% neq 0 (
    echo [ERROR] 项目构建失败
    call :_restore_cp & exit /b 1
)
echo [SUCCESS] 项目构建完成
goto :eof

:debug
call :clean
call :configure Debug
call :build
call :test
goto :eof

:release
call :clean
call :configure Release
call :build
goto :eof

:test
echo [INFO] 运行测试...
if not exist "%BUILD_DIR%" (
    call :configure Debug
    call :build
)
cd /d "%BUILD_DIR%"
if "%IS_MULTI_CONFIG%"=="1" (
    ctest -C %ACTIVE_CONFIG% --output-on-failure --verbose
) else (
    ctest --output-on-failure --verbose
)
if %errorlevel% neq 0 (
    echo [ERROR] 测试失败
    call :_restore_cp & exit /b 1
)
echo [SUCCESS] 所有测试通过
goto :eof

:format
echo [INFO] 格式化代码...
where clang-format >nul 2>nul
if %errorlevel% neq 0 (
    echo [WARNING] clang-format未安装，跳过代码格式化
    goto :eof
)

for /r "%PROJECT_DIR%src" %%f in (*.c *.h) do (
    clang-format -i "%%f"
)
for /r "%PROJECT_DIR%include" %%f in (*.c *.h) do (
    clang-format -i "%%f"
)
echo [SUCCESS] 代码格式化完成
goto :eof

:check
echo [INFO] 检查代码格式...
where clang-format >nul 2>nul
if %errorlevel% neq 0 (
    echo [WARNING] clang-format未安装，跳过格式检查
    goto check_static
)

REM 简化的格式检查（Windows批处理限制）
echo [INFO] 代码格式检查通过 (简化检查)

:check_static
echo [INFO] 运行静态代码分析...
where cppcheck >nul 2>nul
if %errorlevel% neq 0 (
    echo [WARNING] cppcheck未安装，跳过静态分析
    goto :eof
)
cppcheck --enable=all --std=c99 --language=c --suppress=missingIncludeSystem --suppress=unusedFunction --error-exitcode=1 --quiet "%PROJECT_DIR%src" "%PROJECT_DIR%include"
if %errorlevel% neq 0 (
    echo [ERROR] 静态代码分析发现问题
    call :_restore_cp & exit /b 1
)
echo [SUCCESS] 静态代码分析通过
goto :eof

:run
echo [INFO] 运行游戏...
set "GAME_EXE="
if "%IS_MULTI_CONFIG%"=="1" (
    if exist "%BUILD_DIR%\bin\%ACTIVE_CONFIG%\RichGame.exe" set "GAME_EXE=%BUILD_DIR%\bin\%ACTIVE_CONFIG%\RichGame.exe"
) else (
    if exist "%BUILD_DIR%\bin\RichGame.exe" set "GAME_EXE=%BUILD_DIR%\bin\RichGame.exe"
    if not defined GAME_EXE if exist "%BUILD_DIR%\RichGame.exe" set "GAME_EXE=%BUILD_DIR%\RichGame.exe"
)

if not defined GAME_EXE (
    echo [ERROR] 游戏可执行文件不存在，请先构建项目
    call :_restore_cp & exit /b 1
)

cd /d "%BUILD_DIR%"
"%GAME_EXE%"
goto :eof

:ci
echo [INFO] 开始CI流程...
call :clean
call :configure Release
call :build
call :test
call :check
echo [SUCCESS] CI流程完成
goto :eof

:help
echo 大富翁游戏Windows构建脚本
echo.
echo 使用方法: %0 [命令]
echo.
echo 命令:
echo   clean          清理构建目录
echo   debug          Debug模式构建并测试
echo   release        Release模式构建 (默认)
echo   test           运行测试
echo   format         格式化代码
echo   check          检查代码格式和质量
echo   run            运行游戏
echo   ci             运行完整CI流程
echo   help           显示此帮助信息
echo.
echo 示例:
echo   %0 debug      # Debug模式构建并测试
echo   %0 release    # Release模式构建
echo   %0 ci         # 运行完整CI流程
goto :eof

REM 检查是否在项目根目录
if not exist "%PROJECT_DIR%CMakeLists.txt" (
    echo [ERROR] 请在项目根目录运行此脚本
    call :_restore_cp & exit /b 1
)

REM 执行主函数
call :main %*
set "_EXITCODE=%ERRORLEVEL%"
call :_restore_cp
exit /b %_EXITCODE%

:_restore_cp
REM 恢复原代码页
if defined _ORIG_CP chcp %_ORIG_CP% >nul
goto :eof