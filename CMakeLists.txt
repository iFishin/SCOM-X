cmake_minimum_required(VERSION 3.16)
project(SCOM VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt 组件
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    SerialPort
)

# 源文件
set(SOURCES
    src/main.cpp
    src/main_window.cpp
    src/serial_port.cpp
)

# 头文件
set(HEADERS
    include/main_window.h
    include/serial_port.h
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::SerialPort
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Windows 特定设置
if(WIN32)
    # 设置 Windows 编译选项
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    
    # MSVC 需要特殊编译选项来支持 C++17 和 Qt
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /permissive- /Zc:__cplusplus)
    endif()
    
    # 设置输出文件
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# 启用测试
enable_testing()

# 添加测试子目录
add_subdirectory(tests)
