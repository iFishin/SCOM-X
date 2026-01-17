#pragma once
#include <QFile>
#include <QApplication>
#include <QHash>

/**
 * @brief 加载扁平化样式主题
 * 
 * 从 resources/style.qss 模板中读取样式定义，
 * 将所有 ${colorName} 占位符替换为实际的十六进制颜色值
 * 
 * 颜色设计原则：
 * - 主色系：用于主要操作和焦点元素
 * - 功能色系：成功/警告/信息/危险，传递不同的含义
 * - 背景色系：清晰的层级差异
 * - 交互色系：悬停、聚焦、选中状态的统一反馈
 */
inline void loadFlatStyle()
{
    // 定义颜色调色板
    static const QHash<QString, QString> palette = {
        // ===== 主色系 =====
        {"primary",        "#00a86b"},      // 绿色 - 主操作按钮
        {"primaryDark",    "#008c5a"},      // 深绿 - 主操作按钮:hover
        {"primaryLight",   "#5fdeaf"},      // 浅绿 - 背景/禁用态轻提示
        
        // ===== 次色系（蓝色） =====
        {"secondary",      "#007bff"},      // 蓝色 - 次要操作、链接
        {"secondaryDark",  "#0056b3"},      // 深蓝 - 次要操作:hover
        {"secondaryLight", "#80c1ff"},      // 浅蓝 - 背景/聚焦反馈
        
        // ===== 成功色系（绿色变体） =====
        {"success",        "#28a745"},      // 成功绿
        {"successDark",    "#1e7e34"},      // 成功深绿
        {"successLight",   "#d4edda"},      // 成功浅绿背景
        
        // ===== 警告色系（橙色） =====
        {"warning",        "#ffc107"},      // 警告橙
        {"warningDark",    "#e0a800"},      // 警告深橙
        {"warningLight",   "#fff3cd"},      // 警告浅橙背景
        
        // ===== 信息色系（浅蓝） =====
        {"info",           "#17a2b8"},      // 信息青
        {"infoDark",       "#117a8b"},      // 信息深青
        {"infoLight",      "#d1ecf1"},      // 信息浅青背景
        
        // ===== 危险色系（红色） =====
        {"danger",         "#dc3545"},      // 红色 - 删除/危险操作
        {"dangerDark",     "#c82333"},      // 深红 - 危险操作:hover
        {"dangerLight",    "#f8d7da"},      // 浅红 - 危险提示背景
        
        // ===== 背景色系 =====
        {"bgBase",         "#f8f9fa"},      // 浅灰背景 - 页面主体
        {"bgCard",         "#ffffff"},      // 白色 - 卡片/输入框
        {"bgHover",        "#f5f5f5"},      // 浅灰 - 悬停背景
        {"bgActive",       "#e9ecef"},      // 中灰 - 活跃/按压背景
        
        // ===== 边框色系 =====
        {"border",         "#dee2e6"},      // 标准边框 - 默认状态
        {"borderDark",     "#ced4da"},      // 深边框 - 聚焦状态
        {"borderLight",    "#e9ecef"},      // 浅边框 - 禁用状态
        
        // ===== 文本色系 =====
        {"text",           "#495057"},      // 标准文本
        {"textTitle",      "#212529"},      // 标题 - 深色
        {"textCaption",    "#6c757d"},      // 说明文字 - 浅色
        {"textPlaceholder","#adb5bd"},      // 占位符文字
        {"textDisabled",   "#9ca3af"},      // 禁用文字
        
        // ===== 交互色系 =====
        {"hover",          "#f0f4f8"},      // 悬停背景 - 轻提示
        {"focus",          "#e3f2fd"},      // 聚焦背景 - 蓝色提示
        {"active",         "#d4edda"},      // 活跃背景 - 绿色提示
        {"selectBg",       "#007bff"},      // 选中背景 - 蓝色
        {"selectText",     "#ffffff"},      // 选中文字 - 白色
        
        // ===== 阴影色系 =====
        {"shadow",         "#00000010"},    // 浅阴影 - 卡片投影
        {"shadowDark",     "#00000020"}     // 深阴影 - 弹框投影
    };

    // 打开 QSS 模板文件
    QFile f(":/styles/style.qss");
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开样式文件: :/styles/style.qss";
        return;
    }

    // 读取 QSS 模板内容
    QString style = QLatin1String(f.readAll());
    f.close();

    // 执行颜色替换：将 ${colorName} 替换为实际颜色值
    for (auto it = palette.begin(); it != palette.end(); ++it) {
        style.replace(QString("${%1}").arg(it.key()), it.value());
    }

    // 应用样式表
    qApp->setStyleSheet(style);
    
    qDebug() << "✓ 样式表加载成功！";
}
