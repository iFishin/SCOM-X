#!/usr/bin/env bash
# 发布脚本 - 自动化发布流程

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查必要工具
check_tools() {
    print_info "检查必要工具..."
    
    if ! command -v git &> /dev/null; then
        print_error "Git 未安装"
        exit 1
    fi
    
    if ! command -v cmake &> /dev/null; then
        print_error "CMake 未安装"
        exit 1
    fi
    
    print_info "所有工具都已安装"
}

# 获取版本号
get_version() {
    grep "project.*VERSION" CMakeLists.txt | grep -oP '\d+\.\d+\.\d+'
}

# 检查工作目录是否干净
check_clean_repo() {
    print_info "检查 Git 仓库状态..."
    
    if ! git diff-index --quiet HEAD --; then
        print_error "工作目录有未提交的改动，请先提交"
        exit 1
    fi
    
    print_info "工作目录干净"
}

# 运行测试
run_tests() {
    print_info "运行测试..."
    
    if [ -d "build" ]; then
        rm -rf build
    fi
    
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build .
    ctest --verbose
    cd ..
    
    print_info "测试通过"
}

# 更新版本号
update_version() {
    local current_version=$(get_version)
    print_info "当前版本: $current_version"
    
    read -p "请输入新版本号 (format: x.y.z): " new_version
    
    if [[ ! $new_version =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
        print_error "版本号格式不正确"
        exit 1
    fi
    
    # 更新 CMakeLists.txt
    sed -i "s/VERSION $current_version/VERSION $new_version/" CMakeLists.txt
    
    print_info "版本号已更新为: $new_version"
}

# 更新 CHANGELOG
update_changelog() {
    local version=$1
    print_info "请更新 CHANGELOG.md"
    
    # 在 Unreleased 部分添加版本标题
    sed -i "s/## \[Unreleased\]/## [Unreleased]\n\n## [$version] - $(date +%Y-%m-%d)/" CHANGELOG.md
    
    print_warn "请手动编辑 CHANGELOG.md 以添加发布说明"
}

# 提交更改
commit_changes() {
    local version=$1
    
    print_info "提交更改..."
    
    git add CMakeLists.txt CHANGELOG.md
    git commit -m "chore: release v$version"
    
    print_info "提交完成"
}

# 创建标签
create_tag() {
    local version=$1
    
    print_info "创建 Git 标签..."
    
    git tag -a "v$version" -m "Release version $version"
    
    print_info "标签已创建: v$version"
}

# 推送更改
push_changes() {
    print_info "推送更改到远程仓库..."
    
    git push origin main
    git push origin --tags
    
    print_info "推送完成"
}

# 主函数
main() {
    print_info "========================================="
    print_info "SCOM 发布脚本"
    print_info "========================================="
    print_info ""
    
    check_tools
    check_clean_repo
    
    # 显示菜单
    echo "选择操作:"
    echo "1. 完整发布流程"
    echo "2. 仅构建"
    echo "3. 运行测试"
    echo "4. 创建标签"
    echo "5. 退出"
    
    read -p "请选择 (1-5): " choice
    
    case $choice in
        1)
            # 完整流程
            update_version
            version=$(get_version)
            
            run_tests
            update_changelog "$version"
            
            read -p "请确认 CHANGELOG.md 已更新 (yes/no): " confirm
            if [ "$confirm" != "yes" ]; then
                print_warn "已取消发布"
                exit 1
            fi
            
            commit_changes "$version"
            create_tag "$version"
            
            read -p "确认推送到远程仓库? (yes/no): " confirm
            if [ "$confirm" = "yes" ]; then
                push_changes
                print_info "发布完成！版本: v$version"
            else
                print_warn "已跳过推送，请手动执行: git push origin main && git push origin --tags"
            fi
            ;;
        2)
            run_tests
            ;;
        3)
            run_tests
            ;;
        4)
            version=$(get_version)
            read -p "确认创建标签 v$version? (yes/no): " confirm
            if [ "$confirm" = "yes" ]; then
                create_tag "$version"
            fi
            ;;
        5)
            print_info "退出"
            exit 0
            ;;
        *)
            print_error "无效选择"
            exit 1
            ;;
    esac
}

main "$@"
