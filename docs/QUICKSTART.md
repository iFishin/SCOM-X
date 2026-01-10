# 快速入门指南

## 5分钟快速开始

### 前置条件

- Windows 10+、Linux 或 macOS
- 已安装 CMake 3.16+
- 已安装 Qt 6.0+
- 已安装 C++ 编译器

### 三个步骤启动

```bash
# 1. 克隆项目
git clone https://github.com/iFishin/SCOM-X.git
cd SCOM-X

# 2. 构建项目
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# 3. 运行程序
./bin/SCOM  # Linux/macOS
.\bin\SCOM.exe  # Windows
```

## 基本使用

### 连接串口

1. 点击**Refresh**按钮刷新可用串口列表
2. 从下拉菜单选择串口（如 COM3）
3. 设置波特率（默认 115200）
4. 点击**Connect**按钮连接

### 发送数据

1. 在"Send Data"区域输入要发送的数据
2. 从"Format"下拉菜单选择格式（ASCII/HEX/UTF-8）
3. 点击**Send**按钮或按 Enter 发送

### 查看接收数据

- 所有接收到的数据会实时显示在"Receive Data"区域
- 每条数据都会显示时间戳和格式信息
- 使用**Clear**按钮清空日志

## 常见任务

### 发送 HEX 数据

1. 选择 "HEX" 格式
2. 输入十六进制数据，用空格分隔：`48 65 6C 6C 6F`
3. 点击 Send

### 添加换行符

- 勾选 "Add Newline (\\n)" 复选框
- 自动在每条消息末尾添加 `\n`

### 保存日志

目前日志显示在窗口中，将来版本会支持导出功能。

## 故障排除

| 问题 | 解决方案 |
|-----|--------|
| 找不到串口 | 检查设备是否连接，驱动程序是否安装 |
| 连接失败 | 确保端口未被其他程序占用 |
| 数据乱码 | 检查波特率是否与设备匹配 |
| 应用无法启动 | 确保 Qt6 SerialPort 模块已安装 |

## 下一步

- 查看 [开发指南](./docs/DEVELOPMENT.md) 了解项目结构
- 查看 [贡献指南](./CONTRIBUTING.md) 参与开发
- 查看 [架构文档](./docs/ARCHITECTURE.md) 理解代码设计

---

需要帮助？提交 [GitHub Issue](https://github.com/iFishin/SCOM-X/issues)
