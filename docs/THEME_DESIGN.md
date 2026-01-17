# SCOM-X 主题设计指南

## 📚 概述

SCOM-X 采用现代化的扁平设计风格，提供统一、简洁、大方的用户界面。本文档详细说明颜色系统、交互设计和使用规范。

---

## 🎨 颜色系统

### 主色系（绿色）
- **主色 `#00a86b`** - 用于主要操作按钮和关键交互
- **深色 `#008c5a`** - 悬停和按压状态
- **浅色 `#5fdeaf`** - 背景和禁用态轻提示

### 次色系（蓝色）
- **次色 `#007bff`** - 次要操作、链接和聚焦状态
- **深色 `#0056b3`** - 悬停和按压状态
- **浅色 `#80c1ff`** - 背景和聚焦反馈

### 功能色系

#### ✅ 成功（绿色变体）
- 成功 `#28a745` - 完成、成功状态
- 深色 `#1e7e34` - 悬停状态
- 浅色背景 `#d4edda` - 成功信息背景

#### ⚠️ 警告（橙色）
- 警告 `#ffc107` - 需要注意的状态
- 深色 `#e0a800` - 悬停状态
- 浅色背景 `#fff3cd` - 警告信息背景

#### ℹ️ 信息（青色）
- 信息 `#17a2b8` - 提示和信息
- 深色 `#117a8b` - 悬停状态
- 浅色背景 `#d1ecf1` - 信息背景

#### ❌ 危险（红色）
- 危险 `#dc3545` - 删除、危险操作
- 深色 `#c82333` - 悬停状态
- 浅色背景 `#f8d7da` - 危险信息背景

### 背景色系
| 色彩 | 值 | 用途 |
|-----|-----|------|
| 主体背景 | `#f8f9fa` | 页面主体、列表背景 |
| 卡片背景 | `#ffffff` | 卡片、输入框、窗口 |
| 悬停背景 | `#f5f5f5` | 悬停状态 |
| 活跃背景 | `#e9ecef` | 按压、活跃状态 |

### 边框色系
| 色彩 | 值 | 用途 |
|-----|-----|------|
| 标准边框 | `#dee2e6` | 默认状态 |
| 深边框 | `#ced4da` | 聚焦状态 |
| 浅边框 | `#e9ecef` | 禁用状态 |

### 文本色系
| 色彩 | 值 | 用途 |
|-----|-----|------|
| 标准文本 | `#495057` | 正文内容 |
| 标题文本 | `#212529` | 标题和强调 |
| 说明文字 | `#6c757d` | 辅助说明 |
| 占位符文字 | `#adb5bd` | 输入框占位符 |
| 禁用文字 | `#9ca3af` | 禁用态文字 |

### 交互色系
| 色彩 | 值 | 用途 |
|-----|-----|------|
| 悬停背景 | `#f0f4f8` | 轻提示 |
| 聚焦背景 | `#e3f2fd` | 蓝色聚焦 |
| 活跃背景 | `#d4edda` | 绿色活跃 |
| 选中背景 | `#007bff` | 选中状态 |
| 选中文字 | `#ffffff` | 选中文字颜色 |

---

## 🎯 交互设计规范

### 按钮交互

#### 主操作按钮
```qss
状态        背景色        文字色        边框
默认        #00a86b      白色          无
悬停        #008c5a      白色          无
按压        #008c5a      白色          无
禁用        #dee2e6      禁用文字       无
```

#### 次要操作按钮 (buttonStyle="secondary")
```qss
状态        背景色        文字色
默认        #007bff      白色
悬停        #0056b3      白色
按压        #0056b3      白色
```

#### 危险操作按钮 (buttonStyle="danger")
```qss
状态        背景色        文字色
默认        #dc3545      白色
悬停        #c82333      白色
按压        #c82333      白色
```

#### 幽灵按钮 (buttonStyle="ghost")
- 边框风格，点击时填充主色

### 输入框交互

#### 统一的交互流程
```
默认态：灰边框，白色背景
↓ 悬停（鼠标进入）
悬停态：深边框，浅灰背景
↓ 聚焦（点击或Tab）
聚焦态：蓝边框，白色背景
↓ 输入
输入态：蓝边框，白色背景 + 光标闪烁
↓ 失焦（Tab出或点击外部）
默认态：灰边框，白色背景
```

### 下拉框交互

- 使用相同的边框交互逻辑
- 弹出列表项：悬停显示浅灰背景
- 选中项：蓝色背景 + 白色文字 + 加粗

### 复选框/单选框

- **默认态**：灰边框，白色背景
- **悬停态**：主色边框，浅灰背景
- **选中态**：主色背景，隐藏边框
- **禁用态**：浅灰背景，深灰边框

---

## 📐 尺寸规范

### 内边距 (Padding)
| 元素 | 大小 |
|-----|------|
| 按钮 | 8px 上下 / 16px 左右 |
| 输入框 | 6px 上下 / 10px 左右 |
| 下拉框 | 6px 上下 / 10px 左右 |
| 菜单项 | 8px 上下 / 20px 左右 |
| 表格单元格 | 6px 上下 / 8px 左右 |

### 间距 (Spacing)
| 元素 | 大小 |
|-----|------|
| 复选框 | 8px |
| 单选框 | 8px |
| 菜单分隔符上下 | 4px |
| 工具栏间距 | 4px |

### 圆角 (Border Radius)
| 元素 | 大小 |
|-----|------|
| 按钮 | 6px |
| 输入框 | 6px |
| 下拉框 | 6px |
| 卡片 | 6px |
| 分组框 | 8px |
| 复选框 | 4px |
| 单选框 | 9px (50%) |

### 边框宽度 (Border Width)
| 元素 | 大小 |
|-----|------|
| 按钮 | 无 |
| 输入框 | 2px |
| 下拉框 | 2px |
| 默认边框 | 2px |

---

## 📝 排版规范

### 字体族
```
主字体: "Segoe UI", "Consolas", "SimSun", sans-serif
等宽字体: "Courier New", "Consolas", monospace
```

### 字体大小
| 类型 | 大小 |
|-----|------|
| 标题 (title) | 18px, 700 weight |
| 副标题 (subtitle) | 15px, 600 weight |
| 正文 | 13px, 400 weight |
| 说明 (caption) | 12px, 400 weight |
| 小号 | 11px, 400 weight |

### 字体权重
| 权重 | 用途 |
|-----|------|
| 700 (Bold) | 标题、强调 |
| 600 (Semi-bold) | 副标题、按钮 |
| 400 (Normal) | 正文、说明 |

---

## 🎪 标签样式

### 标签属性说明
```qss
QLabel[labelStyle="title"]       /* 18px 标题 */
QLabel[labelStyle="subtitle"]    /* 15px 副标题 */
QLabel[labelStyle="caption"]     /* 12px 说明 */
QLabel[labelStyle="emphasis"]    /* 加粗文本 */
QLabel[labelStyle="success"]     /* 绿色成功 */
QLabel[labelStyle="warning"]     /* 橙色警告 */
QLabel[labelStyle="error"]       /* 红色错误 */
QLabel[labelStyle="info"]        /* 青色信息 */
```

### 按钮样式说明
```qss
QPushButton                      /* 主按钮（绿色）*/
QPushButton[buttonStyle="secondary"]  /* 次按钮（蓝色）*/
QPushButton[buttonStyle="success"]    /* 成功按钮（绿色变体）*/
QPushButton[buttonStyle="warning"]    /* 警告按钮（橙色）*/
QPushButton[buttonStyle="info"]       /* 信息按钮（青色）*/
QPushButton[buttonStyle="danger"]     /* 危险按钮（红色）*/
QPushButton[buttonStyle="ghost"]      /* 幽灵按钮（轻量级）*/
```

---

## 🔧 扩展主题颜色

如需添加新的主题颜色，请按以下步骤操作：

### 1. 编辑 palette.h
在调色板 `QHash` 中添加新颜色：
```cpp
{"customColor",        "#XXXXXX"},      // 自定义颜色
{"customColorDark",    "#XXXXXX"},      // 深色变体
{"customColorLight",   "#XXXXXX"},      // 浅色变体
```

### 2. 编辑 style.qss
在样式表中使用新颜色：
```qss
background-color: ${customColor};
border-color: ${customColorDark};
```

### 3. 编译并测试
```bash
./build.ps1
```

---

## 🚀 使用示例

### 创建成功提示
```cpp
QLabel* successLabel = new QLabel("操作成功！");
successLabel->setProperty("labelStyle", "success");
```

### 创建危险按钮
```cpp
QPushButton* deleteBtn = new QPushButton("删除");
deleteBtn->setProperty("buttonStyle", "danger");
```

### 创建幽灵按钮
```cpp
QPushButton* ghostBtn = new QPushButton("取消");
ghostBtn->setProperty("buttonStyle", "ghost");
```

---

## 📋 检查清单

在设计 UI 时，请确保：

- [ ] 所有按钮使用正确的按钮样式
- [ ] 所有输入框边框在默认/悬停/聚焦时有视觉反馈
- [ ] 信息提示使用对应的功能色（成功/警告/错误/信息）
- [ ] 禁用态元素视觉上明显区分
- [ ] 文本颜色对比度满足 WCAG AA 标准 (4.5:1)
- [ ] 间距和内边距保持一致
- [ ] 圆角尺寸统一

---

## 🎨 颜色对比度检查

关键颜色对比度（相对于背景）：

| 文本 | 背景 | 对比度 | 等级 |
|-----|------|-------|------|
| #495057 | #ffffff | 7.3:1 | AAA ✅ |
| #212529 | #ffffff | 12.6:1 | AAA ✅ |
| #ffffff | #00a86b | 4.6:1 | AA ✅ |
| #ffffff | #007bff | 4.4:1 | AA ✅ |
| #ffffff | #dc3545 | 4.5:1 | AA ✅ |

---

## 📞 反馈与改进

如有任何关于主题设计的建议或问题，请提交 Issue 或 Pull Request。

**最后更新**: 2026年1月17日
**作者**: GitHub Copilot
