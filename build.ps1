#!/usr/bin/env pwsh

# SCOM-X 编译脚本

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# 设置编译工具路径
$cmakePath = "D:\Qt\Tools\CMake_64\bin"
$ninjaPath = "D:\Qt\Tools\Ninja"
$mingwPath = "D:\Qt\Tools\mingw1310_64\bin"

# 添加到环境变量
$env:PATH = "$cmakePath;$ninjaPath;$mingwPath;$env:PATH"

# 进入 build 目录
Push-Location "$scriptDir\build"

Write-Host "[*] 开始编译 SCOM-X..." -ForegroundColor Cyan
Write-Host "工作目录: $(Get-Location)" -ForegroundColor Gray

# 执行编译
$buildResult = cmake --build . --config Release 2>&1

# 检查编译结果
if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "[+] 编译成功！" -ForegroundColor Green
    Write-Host "[+] 可执行文件: $scriptDir\build\bin\SCOM.exe" -ForegroundColor Green
    
    # 询问是否运行程序
    $runApp = Read-Host "是否立即运行程序？(Y/n)"
    if ($runApp -ne 'n' -and $runApp -ne 'N') {
        & "$scriptDir\build\bin\SCOM.exe"
    }
} else {
    Write-Host ""
    Write-Host "[-] 编译失败！" -ForegroundColor Red
    Write-Host "错误代码: $LASTEXITCODE" -ForegroundColor Red
    Write-Host ""
    Write-Host "编译输出:" -ForegroundColor Yellow
    Write-Host $buildResult
    Read-Host "按 Enter 键继续..."
}

Pop-Location
