@echo off
REM 发布脚本 - Windows 版本

setlocal enabledelayedexpansion

REM 颜色定义（仅限 Windows 10+）
set GREEN=[32m
set YELLOW=[33m
set RED=[31m
set NC=[0m

REM 打印消息函数
echo SCOM Release Script for Windows
echo ==================================
echo.

REM 检查必要工具
echo [*] 检查必要工具...
where git >nul 2>nul
if %errorlevel% neq 0 (
    echo [!] Git 未安装
    exit /b 1
)

where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo [!] CMake 未安装
    exit /b 1
)

echo [+] 所有工具都已安装
echo.

REM 菜单
:menu
echo 选择操作:
echo 1. 构建项目
echo 2. 运行测试
echo 3. 清理构建
echo 4. 显示版本
echo 5. 退出
echo.

set /p choice=请选择 (1-5): 

if "%choice%"=="1" goto build
if "%choice%"=="2" goto test
if "%choice%"=="3" goto clean
if "%choice%"=="4" goto version
if "%choice%"=="5" goto end
goto menu

:build
echo [*] 开始构建...
if exist build (
    rmdir /s /q build
)
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
cd ..
echo [+] 构建完成
echo.
goto menu

:test
echo [*] 运行测试...
if not exist build (
    echo [!] 构建目录不存在，请先构建
    goto menu
)
cd build
ctest --verbose
cd ..
echo.
goto menu

:clean
echo [*] 清理构建...
if exist build (
    rmdir /s /q build
    echo [+] 构建目录已删除
)
echo.
goto menu

:version
echo [*] 显示版本信息...
findstr /R "project.*VERSION" CMakeLists.txt
echo.
goto menu

:end
echo [+] 退出
endlocal
