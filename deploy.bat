@echo off
REM 部署脚本 - 将 Qt DLL 复制到 bin 目录

setlocal enabledelayedexpansion

set QT_BIN=D:\Qt\6.10.1\mingw_64\bin
set APP_BIN=%~dp0bin

echo.
echo ========================================
echo  SCOM-X Qt 运行时部署脚本
echo ========================================
echo.

if not exist "%APP_BIN%" (
    echo ❌ 错误: bin 目录不存在
    echo 请先运行: cmake --build . --config Release
    exit /b 1
)

echo 📋 复制必需的 Qt DLL...

REM 核心库
set "DLLS=Qt6Core Qt6Gui Qt6Widgets Qt6SerialPort"

REM 支持库
set "DLLS=!DLLS! libstdc++-6 libwinpthread-1 libgcc_s_seh-1"

for %%D in (!DLLS!) do (
    if exist "!QT_BIN!\%%D.dll" (
        echo   📄 复制 %%D.dll
        copy /Y "!QT_BIN!\%%D.dll" "!APP_BIN!" > nul
    )
)

REM 复制 plugins 目录
set "PLUGINS_SRC=%QT_BIN%..\plugins"
set "PLUGINS_DST=%APP_BIN%\plugins"

if exist "!PLUGINS_SRC!" (
    echo 📁 复制 plugins 目录...
    if not exist "!PLUGINS_DST!" (
        mkdir "!PLUGINS_DST!"
    )
    
    REM 复制平台库
    if exist "!PLUGINS_SRC!\platforms" (
        xcopy /Y /E "!PLUGINS_SRC!\platforms" "!PLUGINS_DST!\platforms" > nul
    )
    
    REM 复制 imageformats
    if exist "!PLUGINS_SRC!\imageformats" (
        xcopy /Y /E "!PLUGINS_SRC!\imageformats" "!PLUGINS_DST!\imageformats" > nul
    )
)

echo.
echo ✅ 部署完成！
echo.
echo 现在可以运行: !APP_BIN!\SCOM.exe
echo.
