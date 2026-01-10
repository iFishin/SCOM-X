# Changelog

所有显著的项目变更将记录在此文件中。

该项目遵循 [Semantic Versioning](https://semver.org/) 版本规范。

## [Unreleased]

### Added
- 基础串口通信功能
- 支持 ASCII 和 HEX 数据格式
- 实时数据发送接收
- 波特率、数据位、停止位、校验位等参数配置
- 接收数据日志记录

### Changed
- 项目从 Python PySide6 迁移到 C++ Qt6

### Fixed
- 无

## [1.0.0] - 2026-01-10

### Added
- 初始发布版本
- 基础串口通信界面
- 支持多种数据格式
- 完整的项目工程结构

---

## 版本命名规则

- **MAJOR.MINOR.PATCH** (如：1.0.0)
- **MAJOR**: 不兼容的 API 变更
- **MINOR**: 向下兼容的功能新增
- **PATCH**: 向下兼容的缺陷修复

## 如何更新此文件

1. 在开发分支上提交代码更改
2. 在 Pull Request 中更新本文件
3. 合并时更新版本号
4. 发布新版本时创建 Git Tag

---

[Unreleased]: https://github.com/iFishin/SCOM-X/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/iFishin/SCOM-X/releases/tag/v1.0.0
