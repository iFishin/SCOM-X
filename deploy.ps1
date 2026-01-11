# SCOM-X Qt Runtime Deployment Script
# 用途: 将 Qt DLL 和插件复制到 bin 目录，使应用能够运行

param(
    [string]$QtPath = "D:\Qt\6.10.1\mingw_64",
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot)
)

$QtBin = Join-Path $QtPath "bin"
$AppBin = Join-Path $ProjectRoot "build\bin"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host " SCOM-X Qt 运行时部署脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 检查目录
if (-not (Test-Path $AppBin)) {
    Write-Host "❌ 错误: bin 目录不存在: $AppBin" -ForegroundColor Red
    Write-Host "请先运行: cmake --build . --config Release" -ForegroundColor Yellow
    exit 1
}

Write-Host "📋 复制必需的 Qt DLL..." -ForegroundColor Green
Write-Host "源目录: $QtBin" -ForegroundColor Gray
Write-Host "目标目录: $AppBin" -ForegroundColor Gray
Write-Host ""

# 需要的 DLL 列表
$CoreDlls = @(
    "Qt6Core",
    "Qt6Gui", 
    "Qt6Widgets",
    "Qt6SerialPort"
)

$SupportDlls = @(
    "libstdc++-6",
    "libwinpthread-1",
    "libgcc_s_seh-1"
)

# 复制 DLL
$AllDlls = $CoreDlls + $SupportDlls
$CopyCount = 0

foreach ($dll in $AllDlls) {
    $Source = Join-Path $QtBin "$dll.dll"
    if (Test-Path $Source) {
        Copy-Item -Path $Source -Destination $AppBin -Force
        Write-Host "  ✅ 复制 $dll.dll" -ForegroundColor Green
        $CopyCount++
    } else {
        Write-Host "  ⚠️  未找到 $dll.dll" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "📁 复制 plugins 目录..." -ForegroundColor Green

# 复制 plugins
$PluginsSrc = Join-Path $QtPath "plugins"
$PluginsDst = Join-Path $AppBin "plugins"

if (Test-Path $PluginsSrc) {
    if (-not (Test-Path $PluginsDst)) {
        New-Item -ItemType Directory -Path $PluginsDst -Force | Out-Null
    }
    
    # 复制 platforms (必需)
    $PlatformSrc = Join-Path $PluginsSrc "platforms"
    if (Test-Path $PlatformSrc) {
        Copy-Item -Path "$PlatformSrc\*" -Destination "$PluginsDst\platforms" -Recurse -Force -ErrorAction SilentlyContinue
        Write-Host "  ✅ 复制 platforms 插件" -ForegroundColor Green
    }
    
    # 复制 imageformats
    $ImageFormatsrc = Join-Path $PluginsSrc "imageformats"
    if (Test-Path $ImageFormatsrc) {
        Copy-Item -Path "$ImageFormatsrc\*" -Destination "$PluginsDst\imageformats" -Recurse -Force -ErrorAction SilentlyContinue
        Write-Host "  ✅ 复制 imageformats 插件" -ForegroundColor Green
    }
    
    # 复制 styles
    $StylesSrc = Join-Path $PluginsSrc "styles"
    if (Test-Path $StylesSrc) {
        Copy-Item -Path "$StylesSrc\*" -Destination "$PluginsDst\styles" -Recurse -Force -ErrorAction SilentlyContinue
        Write-Host "  ✅ 复制 styles 插件" -ForegroundColor Green
    }
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "✅ 部署完成！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "📦 已复制 $CopyCount 个 DLL" -ForegroundColor Cyan
Write-Host "📁 已复制 plugins 目录" -ForegroundColor Cyan
Write-Host ""
Write-Host "现在可以运行应用:" -ForegroundColor Yellow
Write-Host "  $AppBin\SCOM.exe" -ForegroundColor Cyan
Write-Host ""
