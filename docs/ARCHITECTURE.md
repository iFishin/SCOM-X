# 代码架构

## 整体架构设计

```
┌─────────────────────────────────────────┐
│          MainWindow (主窗口)             │
│  - UI 组件管理                          │
│  - 用户交互处理                         │
│  - 设置管理                             │
└─────────────────┬───────────────────────┘
                  │ 使用
                  ▼
┌─────────────────────────────────────────┐
│          SerialPort (串口类)             │
│  - 打开/关闭串口                        │
│  - 数据发送/接收                        │
│  - 格式转换                             │
│  - 错误处理                             │
└─────────────────┬───────────────────────┘
                  │ 使用
                  ▼
┌─────────────────────────────────────────┐
│       QSerialPort (Qt 串口库)            │
│  - 低级串口操作                         │
└─────────────────────────────────────────┘
```

## 类设计

### SerialPort 类

**职责:**
- 管理串口连接
- 处理数据发送和接收
- 转换数据格式
- 发出数据和错误信号

**主要方法:**
- `open()` - 打开串口
- `close()` - 关闭串口
- `write()` - 发送数据
- `read()` - 读取数据
- 静态方法用于数据转换

**信号:**
- `dataReceived` - 数据接收
- `dataSent` - 数据发送
- `errorOccurred` - 错误发生
- `connectionStatusChanged` - 连接状态改变

### MainWindow 类

**职责:**
- 构建用户界面
- 处理用户交互
- 管理应用设置
- 显示通信日志

**主要方法:**
- `setupUI()` - 构建 UI
- `connectSignals()` - 连接信号
- `onConnectClicked()` - 处理连接按钮
- `onSendClicked()` - 处理发送按钮
- `onDataReceived()` - 处理接收到的数据

## 数据流

### 发送数据流

```
用户输入
  ↓
点击发送按钮
  ↓
MainWindow::onSendClicked()
  ↓
SerialPort::write()
  ↓
检查数据格式，转换数据
  ↓
QSerialPort::write()
  ↓
发出 dataSent 信号
  ↓
MainWindow::onDataSent()
  ↓
显示日志
```

### 接收数据流

```
QSerialPort 接收到数据
  ↓
触发 readyRead() 信号
  ↓
SerialPort::onDataReceived()
  ↓
读取数据并格式化
  ↓
发出 dataReceived 信号
  ↓
MainWindow::onDataReceived()
  ↓
显示日志和更新统计
```

## 信号/槽关系

```
SerialPort
  ├─ dataReceived ────────→ MainWindow::onDataReceived()
  ├─ dataSent ────────────→ MainWindow::onDataSent()
  ├─ errorOccurred ───────→ MainWindow::onError()
  └─ connectionStatusChanged ──→ MainWindow::onConnectionStatusChanged()

MainWindow (内部)
  ├─ connectButton::clicked ──→ onConnectClicked()
  ├─ sendButton::clicked ─────→ onSendClicked()
  ├─ clearButton::clicked ────→ onClearClicked()
  ├─ refreshButton::clicked ──→ onRefreshPorts()
  └─ receiveText::textChanged ──→ onReceiveTextChanged()
```

## 扩展点

### 添加新的数据格式

1. 在 `SerialPort::DataFormat` 枚举中添加新格式
2. 在 `SerialPort::write()` 中添加格式转换
3. 在 `SerialPort::onDataReceived()` 中添加显示格式

### 添加新的串口功能

1. 在 `SerialPort` 中添加新方法
2. 如需 UI，在 `MainWindow` 中添加相应 UI 组件
3. 连接信号和槽

### 添加新的 UI 功能

1. 在 `MainWindow::setupUI()` 中创建 UI 组件
2. 在 `MainWindow::connectSignals()` 中连接信号
3. 实现对应的槽函数

## 设计模式应用

1. **观察者模式** - 使用 Qt 的信号/槽机制
2. **单一职责原则** - SerialPort 只处理串口，MainWindow 只处理 UI
3. **工厂模式** - 静态方法用于创建数据和扫描端口
4. **智能指针** - 使用 `std::unique_ptr` 自动管理资源

## 编码规范

- 成员变量以 `m_` 前缀
- 私有成员函数以 `on` 开头的通常是槽函数
- 提供详细的 Doxygen 注释
- 遵循 Qt 编码风格

---

更新时间: 2026-01-10
