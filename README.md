# SCOM - Serial Communication Tool (C++/Qt 版本)

<div align="center">

![C++](https://img.shields.io/badge/C%2B%2B-17-brightgreen)
![Qt](https://img.shields.io/badge/Qt-6.0+-brightgreen)
![License](https://img.shields.io/badge/License-MIT-green)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-blue)

**一个现代化、高性能的串口通信工具**

[English](./README_EN.md) | 中文

</div>

## 📋 目录

- [简介](#简介)
- [系统要求](#系统要求)
- [快速开始](#快速开始)
- [功能特性](#功能特性)
- [项目结构](#项目结构)
- [开发指南](#开发指南)
- [贡献指南](#贡献指南)
- [许可证](#许可证)

## 简介

**SCOM** 是 Python PySide6 版本的 C++/Qt 重构版本，专为需要与串口设备进行高效交互的开发者设计。本版本用 C++ 和 Qt6 实现，提供了更好的性能和跨平台支持。

### 🎯 核心特性

- ✅ **高性能串口通信** - 基于 Qt6 SerialPort 模块
- ✅ **多数据格式支持** - ASCII、HEX、UTF-8
- ✅ **实时日志记录** - 完整的发送/接收记录
- ✅ **灵活配置** - 支持自定义波特率、数据位等
- ✅ **现代化界面** - 基于 Qt6 的响应式设计
- ✅ **跨平台支持** - Windows、Linux、macOS
- ✅ **易于扩展** - 模块化设计，便于二次开发

## 系统要求

### 必需

- **C++ 编译器**
  - Windows: MSVC 2019+ (Visual Studio 2019+)
  - Linux: GCC 11+ 或 Clang 13+
  - macOS: Clang 13+

- **CMake**: 3.16 或更高版本
- **Qt**: 6.0 LTS 或更高版本（需要 Serial Port 模块）
- **Git**: 最新版本

### 可选

- Qt Creator (用于IDE开发)
- Doxygen (用于生成文档)
- Clang-Format (用于代码格式化)

## 快速开始

### Windows

#### 1. 安装依赖

```powershell
# 使用 Chocolatey（如果已安装）
choco install cmake git visualstudio2022community

# 或从官方网站下载安装
# - CMake: https://cmake.org/download/
# - Visual Studio: https://visualstudio.microsoft.com/
# - Qt: https://www.qt.io/download-open-source
```

#### 2. 克隆项目

```powershell
git clone https://github.com/iFishin/SCOM-X.git
cd SCOM-X
```

#### 3. 构建项目

```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

#### 4. 运行程序

```powershell
.\bin\SCOM.exe
```

### Linux (Ubuntu/Debian)

#### 1. 安装依赖

```bash
sudo apt-get update
sudo apt-get install build-essential cmake git
sudo apt-get install qt6-base-dev libqt6serialport6-dev
```

#### 2. 克隆项目

```bash
git clone https://github.com/iFishin/SCOM-X.git
cd SCOM-X
```

#### 3. 构建项目

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

#### 4. 运行程序

```bash
./bin/SCOM
```

### macOS

#### 1. 安装依赖

```bash
# 使用 Homebrew
brew install cmake git qt@6

# 或手动下载 Qt6
```

#### 2. 克隆项目

```bash
git clone https://github.com/iFishin/SCOM-X.git
cd SCOM-X
```

#### 3. 构建项目

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(sysctl -n hw.ncpu)
```

#### 4. 运行程序

```bash
./bin/SCOM
```

## 功能特性

### 基础功能

| 功能 | 描述 | 状态 |
|-----|------|------|
| 端口扫描 | 自动检测可用串口 | ✅ |
| 参数配置 | 波特率、数据位、停止位等 | ✅ |
| 数据发送 | 支持多种格式 | ✅ |
| 数据接收 | 实时显示 | ✅ |
| 日志记录 | 完整的收发日志 | ✅ |
| 格式转换 | ASCII/HEX/UTF-8 | ✅ |

### 高级功能（计划）

- 📋 自定义指令快捷按钮
- 💾 通信记录导出
- 📊 数据分析和可视化
- 🎨 主题定制
- 🔌 插件系统

## 项目结构

```
SCOM-X/
├── include/                    # 头文件
│   ├── serial_port.h          # 串口通信类
│   ├── main_window.h          # 主窗口类
│   └── ...
├── src/                        # 源文件
│   ├── main.cpp               # 程序入口
│   ├── serial_port.cpp        # 串口实现
│   ├── main_window.cpp        # 窗口实现
│   └── ...
├── ui/                         # UI 文件
├── resources/                  # 资源文件（图标、图片等）
├── tests/                      # 单元测试
├── docs/                       # 文档
│   ├── DEVELOPMENT.md         # 开发指南
│   └── ...
├── config/                     # 配置文件
├── scripts/                    # 构建和发布脚本
│   ├── release.sh            # Linux/macOS 发布脚本
│   └── release.bat           # Windows 发布脚本
├── .github/workflows/          # GitHub Actions 配置
│   ├── build.yml             # 构建工作流
│   ├── code-quality.yml      # 代码质量检查
│   └── release.yml           # 发布工作流
├── CMakeLists.txt             # CMake 配置
├── CHANGELOG.md               # 变更日志
├── CONTRIBUTING.md            # 贡献指南
└── README.md                  # 项目说明
```

## 使用示例

### 基本串口通信

```cpp
#include "serial_port.h"

SerialPort port;

// 打开串口
if (port.open("COM3", 115200))
{
    // 发送 ASCII 数据
    port.write("Hello", SerialPort::DataFormat::ASCII);
    
    // 或发送 HEX 数据
    port.write("48 65 6C 6C 6F", SerialPort::DataFormat::HEX);
}

// 连接信号
connect(&port, &SerialPort::dataReceived, this, [](const QString &data) {
    qDebug() << "Received:" << data;
});
```

## 开发指南

### 编译和运行

详见 [开发指南](./docs/DEVELOPMENT.md)

### 代码规范

- 遵循 C++ 17 标准
- 使用 4 个空格缩进
- 遵循 Qt 命名规范
- 添加 Doxygen 风格的注释

### 贡献流程

1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'feat: add amazing feature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

详见 [贡献指南](./CONTRIBUTING.md)

## 构建和发布

### 开发版本构建

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### 发布版本构建

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### 自动化发布

#### Linux/macOS

```bash
chmod +x scripts/release.sh
./scripts/release.sh
```

#### Windows

```powershell
.\scripts\release.bat
```

## CI/CD 流程

本项目使用 GitHub Actions 实现自动化：

- **build.yml** - 在每次提交时自动构建（Windows/Linux/macOS）
- **code-quality.yml** - 代码质量检查（clang-tidy、cppcheck）
- **release.yml** - 发布流程自动化

## 常见问题

### Q: 编译时找不到 Qt？
**A:** 确保 Qt6 已正确安装，并设置 Qt6_DIR 环境变量：
```bash
# Linux/macOS
export Qt6_DIR=/path/to/Qt/6.x.x/gcc_64

# Windows (PowerShell)
$env:Qt6_DIR = "C:\Qt\6.x.x\msvc2022_64"
```

### Q: 如何在 Visual Studio 中打开项目？
**A:** 使用 CMake 插件打开 CMakeLists.txt：
1. Visual Studio → File → Open → CMake
2. 选择 CMakeLists.txt
3. 自动生成解决方案

### Q: 如何添加新的串口功能？
**A:** 编辑 `include/serial_port.h` 和 `src/serial_port.cpp`，遵循项目规范。

## 相关链接

- 🔗 [原始 Python 项目](https://github.com/iFishin/scom)
- 📖 [Qt 官方文档](https://doc.qt.io/)
- 📚 [CMake 文档](https://cmake.org/documentation/)
- 🐛 [问题反馈](https://github.com/iFishin/SCOM-X/issues)

## 许可证

本项目采用 [MIT 许可证](LICENSE) - 详见 LICENSE 文件

---

<div align="center">

Made with ❤️ by [iFishin](https://github.com/iFishin)

如果这个项目对你有帮助，请考虑给它一个 ⭐

</div>
