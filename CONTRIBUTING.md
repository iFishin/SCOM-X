# Contributing Guide

感谢你对 SCOM 项目的贡献！本文档提供了贡献流程和规范。

## 贡献流程

### 1. Fork 和 Clone

```bash
# Fork 项目到你的 GitHub 账户
git clone https://github.com/YOUR_USERNAME/SCOM-X.git
cd SCOM-X
```

### 2. 创建功能分支

```bash
# 创建并切换到新分支
git checkout -b feature/your-feature-name

# 或修复问题
git checkout -b bugfix/issue-description

# 或改进文档
git checkout -b docs/documentation-improvement
```

### 3. 提交更改

- 确保代码遵循项目规范
- 添加必要的注释和文档
- 运行测试确保功能正常

```bash
# 提交代码
git commit -m "feat: 添加新功能描述"
```

### 4. 提交 Pull Request

- Push 到你的 fork
- 在 GitHub 上创建 Pull Request
- 填写 PR 模板中的所有信息
- 等待代码审查

## 分支命名规范

- `feature/*` - 新功能开发
- `bugfix/*` - 缺陷修复
- `docs/*` - 文档更新
- `refactor/*` - 代码重构
- `test/*` - 测试相关
- `chore/*` - 构建、依赖等维护工作

## Commit 消息规范

遵循 [Conventional Commits](https://www.conventionalcommits.org/) 规范：

```
<type>[optional scope]: <description>

[optional body]

[optional footer]
```

### Type 类型：
- `feat`: 新功能
- `fix`: 缺陷修复
- `docs`: 文档更新
- `style`: 代码格式调整（不改变功能）
- `refactor`: 代码重构
- `perf`: 性能优化
- `test`: 测试相关
- `chore`: 构建、依赖等维护工作
- `ci`: CI/CD 配置修改

### 示例：

```
feat(serial-port): 添加十六进制数据支持

- 实现 HEX 格式的数据发送和接收
- 添加数据格式转换函数
- 更新 UI 控件以支持格式选择

Closes #123
```

## 代码规范

### C++ 规范

1. **命名规则**
   - 类名：PascalCase (如 `MainWindow`, `SerialPort`)
   - 函数名：camelCase (如 `openPort`, `readData`)
   - 常量：UPPER_SNAKE_CASE (如 `DEFAULT_BAUD_RATE`)
   - 成员变量：m_camelCase (如 `m_serialPort`)
   - 私有成员：m_开头，后接 camelCase

2. **注释**
   - 使用 Doxygen 格式注释
   - 为所有公共函数添加文档注释
   - 解释复杂逻辑

3. **代码风格**
   - 使用 4 个空格缩进
   - 最大行长度 100 字符
   - 遵循 Qt 编码风格

### 头文件示例：

```cpp
/**
 * @class MyClass
 * @brief 类的简要描述
 *
 * 更详细的描述...
 */
class MyClass : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 函数简要描述
     * @param param1 参数1的描述
     * @return 返回值描述
     */
    void myFunction(const QString &param1);

private:
    std::unique_ptr<QSerialPort> m_serialPort;
};
```

## 测试

- 在提交前运行所有测试
- 为新功能添加相应的测试用例
- 确保测试覆盖率不低于现有水平

```bash
mkdir build
cd build
cmake ..
cmake --build .
ctest
```

## 文档

- 更新相关文档
- 在 `docs/` 目录下添加新文档
- 使用 Markdown 格式

## Pull Request 模板

```markdown
## 描述

请简要描述你的更改内容。

## 相关问题

Closes #(issue number)

## 更改类型

- [ ] 新功能
- [ ] 缺陷修复
- [ ] 重大变更
- [ ] 文档更新

## 测试

描述你所做的测试...

## 检查清单

- [ ] 我的代码遵循项目的代码规范
- [ ] 我已更新相关文档
- [ ] 我为功能添加了测试
- [ ] 新旧测试都能通过
- [ ] 我的更改不引入新的编译警告
```

## 代码审查

- 至少需要一个 maintainer 的审查
- 保持开放的交流态度
- 根据反馈进行调整

## License

通过提交代码，你同意你的代码将在项目的 MIT License 下发布。

## 问题反馈

- 使用 [GitHub Issues](https://github.com/iFishin/SCOM-X/issues) 报告 bug
- 使用清晰的标题和详细的描述
- 包含复现步骤和预期行为

## 联系方式

- 项目主页: https://github.com/iFishin/SCOM-X
- 问题反馈: [Issues](https://github.com/iFishin/SCOM-X/issues)

感谢你的贡献！
