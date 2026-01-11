@echo off
setlocal enabledelayedexpansion

REM 设置Qt和编译工具路径
set "CMAKE_PATH=D:\Qt\Tools\CMake_64\bin"
set "NINJA_PATH=D:\Qt\Tools\Ninja"
set "MINGW_PATH=D:\Qt\Tools\mingw1310_64\bin"

REM 添加到PATH
set "PATH=!CMAKE_PATH!;!NINJA_PATH!;!MINGW_PATH!;!PATH!"

REM 进入 build 目录
cd /d "%~dp0build"

REM 执行编译
echo [*] 开始编译 SCOM-X...
cmake --build . --config Release

if %errorlevel% equ 0 (
    echo.
    echo [+] 编译成功！
    echo [+] 可执行文件位置: bin\SCOM.exe
) else (
    echo.
    echo [-] 编译失败！错误代码: %errorlevel%
    pause
)

endlocal
