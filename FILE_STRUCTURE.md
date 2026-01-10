# SCOM-X 项目文件树

```
SCOM-X/
│
├── 📁 .github/
│   └── 📁 workflows/
│       ├── build.yml                    # 自动构建工作流
│       ├── code-quality.yml             # 代码质量检查
│       └── release.yml                  # 发布工作流
│
├── 📁 include/
│   ├── serial_port.h                    # 串口通信类 (420 行)
│   └── main_window.h                    # 主窗口类 (200 行)
│
├── 📁 src/
│   ├── main.cpp                         # 程序入口 (30 行)
│   ├── serial_port.cpp                  # 串口实现 (480 行)
│   └── main_window.cpp                  # 窗口实现 (580 行)
│
├── 📁 ui/
│   └── [UI 文件目录 - 待扩展]
│
├── 📁 resources/
│   └── [资源文件目录 - 图标、图片等]
│
├── 📁 tests/
│   └── CMakeLists.txt                   # 测试配置
│
├── 📁 docs/
│   ├── DEVELOPMENT.md                   # 开发指南 (420 行)
│   ├── ARCHITECTURE.md                  # 架构设计 (180 行)
│   └── QUICKSTART.md                    # 快速开始 (100 行)
│
├── 📁 config/
│   └── build.ini                        # 构建配置
│
├── 📁 scripts/
│   ├── release.sh                       # Linux/macOS 发布脚本
│   └── release.bat                      # Windows 发布脚本
│
├── 📁 examples/
│   └── serial_port_example.cpp          # 使用示例 (100 行)
│
├── 📁 build/                            # 构建目录 (输出)
│
├── CMakeLists.txt                       # CMake 主配置 (70 行)
├── README_FULL.md                       # 完整说明 (320 行)
├── README.md                            # 简要说明 (重定向到上文)
├── CHANGELOG.md                         # 变更日志 (60 行)
├── CONTRIBUTING.md                      # 贡献指南 (250 行)
├── LICENSE                              # MIT 许可证
├── .gitignore                           # Git 忽略规则
├── TODO.md                              # 项目计划 (120 行)
├── PROJECT_SUMMARY.md                   # 项目总结 (300 行)
└── COMPLETION_REPORT.md                 # 完成报告 (400 行)
```

## 📊 文件统计

### 代码文件
```
include/serial_port.h          420 行 ▓▓▓▓▓
include/main_window.h          200 行 ▓▓▓
src/main.cpp                    30 行 ▓
src/serial_port.cpp           480 行 ▓▓▓▓▓
src/main_window.cpp           580 行 ▓▓▓▓▓
examples/serial_port_example.cpp 100 行 ▓▓

总计:                        1,810 行
```

### 配置文件
```
CMakeLists.txt                 70 行  ▓▓
tests/CMakeLists.txt           20 行  ▓
config/build.ini               30 行  ▓
.github/workflows/build.yml    80 行  ▓▓
.github/workflows/code-quality.yml 50 行 ▓
.github/workflows/release.yml 150 行  ▓▓▓
.gitignore                     80 行  ▓▓

总计:                         480 行
```

### 文档文件
```
README_FULL.md                320 行  ▓▓▓▓▓
CONTRIBUTING.md               250 行  ▓▓▓▓
docs/DEVELOPMENT.md           420 行  ▓▓▓▓▓
docs/ARCHITECTURE.md          180 行  ▓▓▓
docs/QUICKSTART.md            100 行  ▓▓
CHANGELOG.md                   60 行  ▓
TODO.md                        120 行  ▓▓
PROJECT_SUMMARY.md            300 行  ▓▓▓▓
COMPLETION_REPORT.md          400 行  ▓▓▓▓▓

总计:                        2,150 行
```

### 脚本文件
```
scripts/release.sh             200 行  ▓▓▓
scripts/release.bat            80 行  ▓▓

总计:                         280 行
```

## 🎯 文件用途速查

### 快速开始 🚀
- `docs/QUICKSTART.md` - 5分钟快速入门
- `README_FULL.md` - 完整的项目说明

### 开发工作 👨‍💻
- `docs/DEVELOPMENT.md` - 详细开发指南
- `docs/ARCHITECTURE.md` - 代码架构说明
- `include/*.h` - 头文件接口
- `src/*.cpp` - 实现代码
- `examples/serial_port_example.cpp` - 使用示例

### 贡献参与 🤝
- `CONTRIBUTING.md` - 贡献流程和规范
- `.github/workflows/*.yml` - CI/CD 工作流

### 项目管理 📊
- `CHANGELOG.md` - 版本历史
- `TODO.md` - 项目计划
- `PROJECT_SUMMARY.md` - 项目总结
- `COMPLETION_REPORT.md` - 完成报告

### 编译构建 🔨
- `CMakeLists.txt` - 构建配置
- `config/build.ini` - 构建参数
- `scripts/release.sh` - Linux/macOS 发布
- `scripts/release.bat` - Windows 发布

---

## 📈 项目增长路线图

### ✅ 已完成 (v1.0.0)
```
src/
  ├── serial_port.cpp      ✅ 基础串口通信
  ├── main_window.cpp      ✅ UI 界面
  └── main.cpp             ✅ 程序入口

include/
  ├── serial_port.h        ✅ 接口定义
  └── main_window.h        ✅ UI 接口

CI/CD                       ✅ 自动化流程
Documentation              ✅ 详尽文档
```

### 📌 计划中 (v1.1-v2.0)
```
features/
  ├── custom_commands      📋 自定义指令
  ├── data_export          📋 记录导出
  ├── visualization        📋 数据可视化
  ├── themes               📋 主题支持
  ├── multi_port           📋 多串口
  └── plugins              📋 插件系统

components/
  ├── data_analyzer        📋 数据分析
  ├── chart_widget         📋 图表组件
  └── script_executor      📋 脚本执行
```

---

## 🏗️ 项目结构说明

### 顶级目录结构

| 目录 | 说明 | 包含内容 |
|-----|------|---------|
| `.github/` | GitHub 配置 | CI/CD 工作流定义 |
| `include/` | 头文件 | 类接口定义 |
| `src/` | 源文件 | 类实现代码 |
| `ui/` | UI 文件 | Qt Designer 文件 |
| `resources/` | 资源文件 | 图标、图片等 |
| `tests/` | 测试代码 | 单元测试 |
| `docs/` | 文档 | 使用和开发文档 |
| `config/` | 配置文件 | 应用配置 |
| `scripts/` | 脚本 | 构建和发布脚本 |
| `examples/` | 示例代码 | 使用示例 |
| `build/` | 构建输出 | 编译产物 |

### 核心类关系

```
QApplication
    └── MainWindow (QMainWindow)
            └── SerialPort (QObject)
                    └── QSerialPort
```

---

## 💾 Git 分支策略

**当前分支**: `main` (主分支)

**推荐分支结构**:
```
main                    # 发布分支
├── feature/*           # 功能分支
├── bugfix/*            # 修复分支
├── docs/*              # 文档分支
└── release/*           # 发布分支
```

---

## 🔄 构建产物

### 构建后的目录结构

```
build/
├── bin/
│   ├── SCOM.exe        # Windows 可执行文件
│   ├── SCOM            # Linux 可执行文件
│   └── SCOM.app/       # macOS 应用包
├── CMakeFiles/
├── CMakeCache.txt
└── cmake_install.cmake
```

---

**最后更新**: 2026-01-10  
**版本**: 1.0.0
