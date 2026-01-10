# 🔨 SCOM-X 编译说明

## 编译环境

项目成功编译需要以下环境：

### 工具链
- **Qt 6.10.1** (mingw_64 版本) - 必须
- **MinGW 13.1.0** - 推荐（与 Qt mingw_64 版本匹配）
- **CMake 3.30+** - 必须
- **Ninja** - 推荐作为构建系统

> ⚠️ **重要**：请勿使用 MSVC（Visual Studio）编译器与 Qt mingw 版本混用，这会导致链接错误。如果需要用 MSVC，需要安装对应的 MSVC 版本 Qt。

## 快速编译步骤

### 1. 准备编译环境

```powershell
# 添加 Qt 工具到环境变量
$env:PATH = "D:\Qt\Tools\CMake_64\bin;D:\Qt\Tools\Ninja;D:\Qt\Tools\mingw1310_64\bin;$env:PATH"

# 验证工具是否就绪
cmake --version
ninja --version
g++ --version
```

### 2. 创建构建目录

```powershell
cd D:\#GIT\SCOM-X
mkdir build -Force
cd build
```

### 3. 配置项目

```powershell
cmake .. -G Ninja `
  -DQt6_DIR=D:/Qt/6.10.1/mingw_64/lib/cmake/Qt6 `
  -DCMAKE_C_COMPILER=D:/Qt/Tools/mingw1310_64/bin/gcc.exe `
  -DCMAKE_CXX_COMPILER=D:/Qt/Tools/mingw1310_64/bin/g++.exe
```

### 4. 编译

```powershell
cmake --build . --config Release
```

### 5. 运行

```powershell
.\bin\SCOM.exe
```

## 完整一键编译脚本

创建文件 `build.ps1`：

```powershell
# 设置环境
$QtPath = "D:\Qt"
$env:PATH = "$QtPath\Tools\CMake_64\bin;$QtPath\Tools\Ninja;$QtPath\Tools\mingw1310_64\bin;$env:PATH"

# 进入项目目录
$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
cd $ProjectRoot

# 清理旧的构建
if (Test-Path "build") {
    Remove-Item -Recurse -Force build
}

# 创建和配置
mkdir build -Force | Out-Null
cd build

Write-Host "🔧 配置 CMake..." -ForegroundColor Green
cmake .. -G Ninja `
  -DQt6_DIR=$QtPath/6.10.1/mingw_64/lib/cmake/Qt6 `
  -DCMAKE_C_COMPILER=$QtPath/Tools/mingw1310_64/bin/gcc.exe `
  -DCMAKE_CXX_COMPILER=$QtPath/Tools/mingw1310_64/bin/g++.exe

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ CMake 配置失败" -ForegroundColor Red
    exit 1
}

Write-Host "🏗️ 编译项目..." -ForegroundColor Green
cmake --build . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ 编译失败" -ForegroundColor Red
    exit 1
}

Write-Host "✅ 编译成功！" -ForegroundColor Green
Write-Host "📦 可执行文件位置: $(Get-Location)\bin\SCOM.exe" -ForegroundColor Cyan

# 提示运行
Write-Host ""
Write-Host "运行应用: .\bin\SCOM.exe" -ForegroundColor Yellow
```

运行脚本：

```powershell
.\build.ps1
```

## 故障排除

### 问题 1: CMake 找不到 Qt6

**症状**: `Could not find a package configuration file provided by "Qt6"`

**解决**:
```powershell
# 确保 Qt6_DIR 指向正确的路径
-DQt6_DIR=D:/Qt/6.10.1/mingw_64/lib/cmake/Qt6
```

### 问题 2: 编译器不匹配

**症状**: `error LNK1181: 无法打开输入文件 "mingw32.lib"`

**解决**: 确保使用 MinGW 编译器，不要混用 MSVC：
```powershell
# ✅ 正确
-DCMAKE_CXX_COMPILER=D:/Qt/Tools/mingw1310_64/bin/g++.exe

# ❌ 错误（与 MSVC 混用）
# 使用 Visual Studio 17 2022 生成器 + mingw Qt
```

### 问题 3: Ninja 找不到

**症状**: `CMake Error: generator : Ninja Does not match the generator used previously`

**解决**: 
```powershell
# 完全清理构建目录
Remove-Item build -Recurse -Force
mkdir build
cd build

# 重新配置
cmake ...
```

### 问题 4: 中文编码警告 (C4819)

这些是警告，不影响编译。如果需要消除，将源文件保存为 UTF-8 with BOM 格式。

## 编译输出说明

成功的编译输出应该如下：

```
[1/6] Automatic MOC and UIC for target SCOM
[2/6] Building CXX object CMakeFiles/SCOM.dir/src/serial_port.cpp.obj
[3/6] Building CXX object CMakeFiles/SCOM.dir/src/main.cpp.obj
[4/6] Building CXX object CMakeFiles/SCOM.dir/SCOM_autogen/mocs_compilation.cpp.obj
[5/6] Building CXX object CMakeFiles/SCOM.dir/src/main_window.cpp.obj
[6/6] Linking CXX executable bin\SCOM.exe
✅ 编译成功！
📦 可执行文件: bin/SCOM.exe (约 465 KB)
```

## 为什么使用 MinGW 而不是 MSVC？

您当前安装的 Qt 版本是 `6.10.1 mingw_64`，这意味着：
- Qt 库是用 MinGW 编译的
- 必须用相同的编译器（MinGW）来链接
- 混用 MSVC 和 mingw Qt 会导致二进制不兼容

如果想用 MSVC，有两种选择：
1. 安装 Qt 的 MSVC 版本（通过 Qt Installer）
2. 继续使用 MinGW（推荐，因为已经安装）

## 下一步

✅ 编译成功后，您可以：

1. **运行应用**
   ```powershell
   .\build\bin\SCOM.exe
   ```

2. **测试功能** - 连接到串口设备测试数据传输

3. **创建发布版本** - 使用 `scripts/release.sh` (Linux/Mac) 或 `scripts/release.bat` (Windows)

4. **开发和调试** - 修改源代码后重新运行 `cmake --build .`

## 环境快速配置

如果频繁编译，建议创建 `setup_env.ps1`：

```powershell
# setup_env.ps1 - 仅需运行一次
$QtPath = "D:\Qt"
$ProfilePath = $PROFILE

if (-not (Test-Path $ProfilePath)) {
    New-Item -ItemType File -Path $ProfilePath -Force | Out-Null
}

$EnvScript = @"
# Qt 编译环境
`$env:PATH = "$QtPath\Tools\CMake_64\bin;$QtPath\Tools\Ninja;$QtPath\Tools\mingw1310_64\bin;`$env:PATH"
Write-Host "✅ Qt 编译环境已加载" -ForegroundColor Green
"@

Add-Content -Path $ProfilePath -Value $EnvScript

Write-Host "✅ 环境配置完成！请重启 PowerShell"
```

## 参考资源

- [Qt 6 文档](https://doc.qt.io/qt-6/)
- [CMake 文档](https://cmake.org/cmake/help/documentation.html)
- [项目完整编译说明](docs/DEVELOPMENT.md)

---

**有问题？**
- 查看 `docs/DEVELOPMENT.md` 获取详细开发指南
- 参考 `CONTRIBUTING.md` 了解项目贡献流程
- 检查 `START_HERE.md` 获取快速开始指南

