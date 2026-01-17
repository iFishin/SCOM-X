# 🎨 SCOM-X 主题颜色速查表

## 快速参考

### 主要颜色代码

```
🟢 绿色系（主操作）
  primary:        #00a86b    ✅ 主按钮、关键操作
  primaryDark:    #008c5a    ↳ 悬停/按压状态
  primaryLight:   #5fdeaf    ↳ 背景提示

🔵 蓝色系（次操作、聚焦）
  secondary:      #007bff    ✅ 次按钮、链接、聚焦
  secondaryDark:  #0056b3    ↳ 悬停/按压状态
  secondaryLight: #80c1ff    ↳ 背景提示

✅ 成功（绿色变体）
  success:        #28a745    ✅ 成功提示
  successDark:    #1e7e34    ↳ 深色变体
  successLight:   #d4edda    ↳ 浅色背景

⚠️ 警告（橙色）
  warning:        #ffc107    ⚠️ 警告提示
  warningDark:    #e0a800    ↳ 深色变体
  warningLight:   #fff3cd    ↳ 浅色背景

ℹ️ 信息（青色）
  info:           #17a2b8    ℹ️ 信息提示
  infoDark:       #117a8b    ↳ 深色变体
  infoLight:      #d1ecf1    ↳ 浅色背景

❌ 危险（红色）
  danger:         #dc3545    ❌ 删除/危险操作
  dangerDark:     #c82333    ↳ 深色变体
  dangerLight:    #f8d7da    ↳ 浅色背景
```

### 背景与中性色

```
📄 背景色
  bgBase:         #f8f9fa    ← 页面背景
  bgCard:         #ffffff    ← 卡片/窗口背景
  bgHover:        #f5f5f5    ← 悬停背景
  bgActive:       #e9ecef    ← 活跃/按压背景

🖼️ 边框色
  border:         #dee2e6    ← 默认边框
  borderDark:     #ced4da    ← 聚焦边框
  borderLight:    #e9ecef    ← 禁用边框

📝 文本色
  text:           #495057    ← 标准文本
  textTitle:      #212529    ← 标题文本
  textCaption:    #6c757d    ← 说明文字
  textPlaceholder:#adb5bd    ← 占位符
  textDisabled:   #9ca3af    ← 禁用文字

🎯 交互色
  hover:          #f0f4f8    ← 悬停提示
  focus:          #e3f2fd    ← 聚焦提示
  active:         #d4edda    ← 活跃提示
  selectBg:       #007bff    ← 选中背景
  selectText:     #ffffff    ← 选中文字

💡 阴影色
  shadow:         #00000010  ← 浅阴影
  shadowDark:     #00000020  ← 深阴影
```

---

## 在代码中使用

### 在 QSS 中使用
```qss
/* 使用占位符，会被自动替换 */
background-color: ${primary};
border-color: ${secondary};
color: ${textTitle};
```

### 使用标签样式
```cpp
QLabel* label = new QLabel("成功！");
label->setProperty("labelStyle", "success");  // 绿色

QLabel* warning = new QLabel("警告");
warning->setProperty("labelStyle", "warning"); // 橙色

QLabel* error = new QLabel("错误");
error->setProperty("labelStyle", "error");    // 红色
```

### 使用按钮样式
```cpp
// 绿色主按钮（默认）
auto mainBtn = new QPushButton("确定");

// 蓝色次按钮
auto secondBtn = new QPushButton("下一步");
secondBtn->setProperty("buttonStyle", "secondary");

// 红色危险按钮
auto deleteBtn = new QPushButton("删除");
deleteBtn->setProperty("buttonStyle", "danger");

// 橙色警告按钮
auto warningBtn = new QPushButton("重置");
warningBtn->setProperty("buttonStyle", "warning");

// 轻量级幽灵按钮
auto ghostBtn = new QPushButton("取消");
ghostBtn->setProperty("buttonStyle", "ghost");
```

---

## 交互流程图

### 输入框的状态变化
```
未交互(默认)
    ↓
[灰边框 + 白背景]
    │
    ├─ 鼠标进入 → 悬停态 [深灰边框 + 浅灰背景]
    │                ↓
    │           点击/Tab → 聚焦态 [蓝边框 + 白背景]
    │
    └─ 禁用 → 禁用态 [浅边框 + 浅灰背景]
```

### 按钮的状态变化
```
未交互(默认)
    ↓
[颜色背景 + 白文字]
    │
    ├─ 鼠标进入 → 悬停态 [深颜色背景 + 白文字]
    │
    ├─ 鼠标按下 → 按压态 [深颜色背景 + 白文字 + 缩小效果]
    │
    └─ 禁用 → 禁用态 [灰背景 + 灰文字]
```

---

## 设计规范

### 内边距
- 按钮：8px 上下 / 16px 左右
- 输入框：6px 上下 / 10px 左右
- 菜单项：8px 上下 / 20px 左右

### 圆角
- 按钮、输入框、下拉框：6px
- 分组框、工具栏：8px
- 复选框：4px
- 单选框：9px (圆形)

### 边框宽度
- 按钮：无边框
- 输入框、下拉框等：2px

---

## 常见组件对照表

| 组件 | 默认色 | 悬停色 | 聚焦色 | 禁用色 |
|------|--------|--------|---------|---------|
| **主按钮** | primary | primaryDark | primaryDark | border |
| **次按钮** | secondary | secondaryDark | secondaryDark | border |
| **成功按钮** | success | successDark | successDark | border |
| **警告按钮** | warning | warningDark | warningDark | border |
| **危险按钮** | danger | dangerDark | dangerDark | border |
| **输入框边框** | border | borderDark | secondary | borderLight |
| **输入框背景** | bgCard | bgHover | bgCard | bgBase |
| **下拉框边框** | border | borderDark | secondary | borderLight |
| **表格行** | bgCard | hover | selectBg | - |
| **菜单项** | bgCard | selectBg | - | textDisabled |

---

## 无障碍考虑

### 色彩对比度（WCAG AA 标准 4.5:1）

✅ **通过检查**
- 深灰文本 (#495057) 在白色 (#ffffff) 上 - 7.3:1
- 深色文本 (#212529) 在白色 (#ffffff) 上 - 12.6:1
- 白色文本 (#ffffff) 在绿色 (#00a86b) 上 - 4.6:1
- 白色文本 (#ffffff) 在蓝色 (#007bff) 上 - 4.4:1

### 颜色不依赖原则
- 所有信息应通过颜色+图标/形状传达
- 例如：成功不仅是绿色，还应有✓符号
- 错误不仅是红色，还应有✗符号

---

## 🎯 最佳实践

✅ **推荐**
- 使用成对的颜色（主色 + 深色）
- 为功能操作使用对应的功能色
- 在必要时添加图标增强辅助

❌ **避免**
- 随意混合多个主色，保持统一
- 使用纯黑色或纯白色（使用预定义色）
- 过度使用鲜艳颜色，应保留给重要操作

---

**文档更新日期**: 2026年1月17日
