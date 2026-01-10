# 开发指南

本文档提供了 SCOM 项目的开发说明。

## 系统要求

- **操作系统**: Windows 10/11（主要），Linux/macOS（可选）
- **C++ 编译器**: MSVC 2019+ 或 GCC 11+
- **CMake**: 3.16 或更高版本
- **Qt**: 6.0 或更高版本
- **Git**: 最新版本

## 开发环境搭建

### Windows 环境

#### 1. 安装依赖工具

```powershell
# 使用 Chocolatey（推荐）
choco install cmake git visualstudio2022community

# 或手动下载安装：
# - CMake: https://cmake.org/download/
# - Visual Studio: https://visualstudio.microsoft.com/
# - Qt: https://www.qt.io/download-open-source
```

#### 2. 安装 Qt

从 [Qt 官方网站](https://www.qt.io/download-open-source) 下载并安装 Qt6。

**安装建议**:
- 选择 6.x LTS 版本
- 选择对应的 MSVC 编译器版本
- 勾选 Qt Serial Port 模块

#### 3. 配置环境变量

```powershell
# 设置 Qt 路径（替换为你的实际安装路径）
$env:Qt6_DIR = "C:\Qt\6.x.x\msvc2022_64"
$env:PATH += ";C:\Qt\6.x.x\msvc2022_64\bin"
```

### Linux/macOS 环境

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake git
sudo apt-get install qt6-base-dev libqt6serialport6-dev

# macOS (使用 Homebrew)
brew install cmake git qt@6
```

## 项目结构

```
SCOM-X/
├── include/              # 头文件
│   ├── serial_port.h    # 串口通信类
│   └── main_window.h    # 主窗口类
├── src/                 # 源文件
│   ├── main.cpp        # 程序入口
│   ├── serial_port.cpp # 串口实现
│   └── main_window.cpp # 主窗口实现
├── ui/                 # UI 文件（.ui）
├── resources/          # 资源文件（图标、图片等）
├── tests/              # 单元测试
├── docs/               # 文档
├── config/             # 配置文件
├── scripts/            # 脚本文件
├── CMakeLists.txt      # CMake 配置
├── CHANGELOG.md        # 变更日志
├── CONTRIBUTING.md     # 贡献指南
└── README.md          # 项目说明
```

## 构建项目

### 使用 CMake（推荐）

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 构建
cmake --build . --config Release

# 运行
./bin/SCOM  # Linux/macOS
.\bin\SCOM.exe  # Windows
```

### 使用 Qt Creator

1. 打开 Qt Creator
2. 打开项目: File → Open File or Project → CMakeLists.txt
3. 配置构建套件（Kit）
4. 构建: Build → Build Project
5. 运行: Build → Run

## 开发工作流

### 1. 启动开发

```bash
# 克隆项目
git clone https://github.com/iFishin/SCOM-X.git
cd SCOM-X

# 创建特性分支
git checkout -b feature/your-feature

# 安装依赖（如果需要）
```

### 2. 编写代码

- 遵循代码规范（见下文）
- 添加适当的注释
- 频繁提交小的、逻辑相关的改动

### 3. 测试代码

```bash
cd build
ctest --verbose
```

### 4. 提交和推送

```bash
git add .
git commit -m "feat: 描述你的改动"
git push origin feature/your-feature
```

### 5. 创建 Pull Request

在 GitHub 上创建 PR，等待代码审查。

## 代码规范

### 命名约定

| 类型 | 规范 | 示例 |
|-----|------|------|
| 类名 | PascalCase | `MainWindow`, `SerialPort` |
| 函数名 | camelCase | `openPort`, `readData` |
| 常量 | UPPER_SNAKE_CASE | `DEFAULT_BAUD_RATE` |
| 全局变量 | g_camelCase | `g_serialPort` |
| 成员变量 | m_camelCase | `m_serialPort`, `m_dataFormat` |
| 局部变量 | camelCase | `tempBuffer`, `index` |

### 代码风格

1. **缩进**: 使用 4 个空格
2. **行长**: 最大 100 字符
3. **括号**: K&R 风格

```cpp
// 好的例子
if (condition)
{
    doSomething();
}
else
{
    doOtherThing();
}

// 不推荐
if (condition) { doSomething(); }
```

### 注释

- 使用 Doxygen 格式注释
- 为公共接口添加详细文档
- 说明为什么，而不是怎样

```cpp
/**
 * @brief 发送数据到串口
 * @param data 要发送的数据字符串
 * @param format 数据格式（ASCII 或 HEX）
 * @return 发送的字节数，失败返回 -1
 * 
 * @details
 * 此函数将根据指定的格式将数据转换为字节流并发送。
 * 如果串口未打开，将返回 -1 并发出 errorOccurred 信号。
 */
qint64 write(const QString &data, DataFormat format = DataFormat::ASCII);
```

## 常见任务

### 添加新功能

1. 创建特性分支: `git checkout -b feature/your-feature`
2. 在适当的文件中添加函数声明和实现
3. 更新相关文档
4. 添加测试用例
5. 提交 Pull Request

### 修复 Bug

1. 创建修复分支: `git checkout -b bugfix/issue-number`
2. 定位并修复问题
3. 添加测试防止回归
4. 提交 Pull Request

### 更新文档

1. 编辑相关的 `.md` 文件
2. 提交: `git commit -m "docs: 更新文档"`

## 调试

### 使用 Qt Creator 调试

1. 设置断点: 点击行号左侧
2. 运行调试: Debug → Start Debugging
3. 使用调试工具栏控制执行流

### 常用快捷键

| 快捷键 | 功能 |
|-------|------|
| F10 | Step Over |
| F11 | Step Into |
| Shift+F11 | Step Out |
| F5 | Continue |
| Ctrl+Shift+F10 | Restart |

### 日志输出

```cpp
#include <QDebug>

qDebug() << "Message:" << variable;
qWarning() << "Warning:" << variable;
qCritical() << "Error:" << variable;
qInfo() << "Info:" << variable;
```

## 性能优化

- 使用智能指针（`std::unique_ptr`、`std::shared_ptr`）
- 避免不必要的内存拷贝，使用引用
- 使用 Qt 的异步 I/O
- 定期检查内存泄漏

## 发布准备

### 版本更新

1. 更新版本号在 `CMakeLists.txt`
2. 更新 `CHANGELOG.md`
3. 创建 Git Tag: `git tag v1.0.0`

### 构建发布版本

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## 常见问题

### Q: Qt 库找不到？
A: 检查 Qt 安装路径，更新 `Qt6_DIR` 环境变量或 CMake 配置。

### Q: 编译错误：找不到 QSerialPort？
A: 确保安装了 Qt Serial Port 模块，或更新 Qt。

### Q: 如何清理构建文件？
A: `rm -rf build` (Linux/macOS) 或 `rmdir /s build` (Windows)

## 有用资源

- [Qt 官方文档](https://doc.qt.io/)
- [CMake 文档](https://cmake.org/documentation/)
- [Modern C++](https://en.cppreference.com/)
- [Conventional Commits](https://www.conventionalcommits.org/)

## 获取帮助

- 查看 [Issues](https://github.com/iFishin/SCOM-X/issues)
- 阅读相关文档
- 提出新问题时，请包含详细信息

---

祝你开发愉快！
