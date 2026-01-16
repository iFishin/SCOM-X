#!/usr/bin/env pwsh

# SCOM-X 编译脚本
# Usage: .\build.ps1 -y              # 如果提供 -y 则在编译成功后自动运行程序
# Usage: .\build.ps1 -clean         # 清理构建文件并重新编译
# Usage: .\build.ps1 -clean -y      # 清理后重新编译并自动运行

param(
    [switch]$y,
    [switch]$clean
)

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# 设置编译工具路径
$cmakePath = "D:\Qt\Tools\CMake_64\bin"
$ninjaPath = "D:\Qt\Tools\Ninja"
$mingwPath = "D:\Qt\Tools\mingw1310_64\bin"
$qtPath = "D:\Qt\6.10.1\mingw_64"

# 添加到环境变量
$env:PATH = "$cmakePath;$ninjaPath;$mingwPath;$env:PATH"
$env:CMAKE_PREFIX_PATH = $qtPath

# 进入 build 目录
Push-Location "$scriptDir\build"

Write-Host "[*] 开始编译 SCOM-X..." -ForegroundColor Cyan
Write-Host "工作目录: $(Get-Location)" -ForegroundColor Gray

# 如果指定了 -clean，执行清理并重新初始化 CMake
if ($clean) {
    Write-Host "[*] 执行清理编译..." -ForegroundColor Yellow
    Pop-Location
    Remove-Item -Recurse -Force "$scriptDir\build\*" -ErrorAction SilentlyContinue
    mkdir "$scriptDir\build" -Force | Out-Null
    Push-Location "$scriptDir\build"
    Write-Host "[*] 重新初始化 CMake..." -ForegroundColor Yellow
    & cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=c++.exe .. 2>&1 | Out-Null
    Write-Host "[+] 清理完成，重新生成项目文件" -ForegroundColor Green
}

# 执行编译
$buildResult = cmake --build . --config Release 2>&1

# 检查编译结果
if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "[+] 编译成功！" -ForegroundColor Green
    Write-Host "[+] 可执行文件: $scriptDir\build\bin\SCOM.exe" -ForegroundColor Green
    
    # 如果传入了 -y，则自动运行；否则询问用户
    if ($y) {
        Write-Host "[*] 参数 -y 检测到：自动运行程序..." -ForegroundColor Cyan
        & "$scriptDir\build\bin\SCOM.exe"
    } else {
        $runApp = Read-Host "是否立即运行程序？(Y/n)"
        if ($runApp -ne 'n' -and $runApp -ne 'N') {
            & "$scriptDir\build\bin\SCOM.exe"
        }
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
