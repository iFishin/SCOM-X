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
    
    # Windows 上的自动部署（复制 Qt DLL 和插件）
    if(Qt6_FOUND)
        # 获取 Qt bin 目录
        get_target_property(Qt6_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
        get_filename_component(Qt6_BIN_DIR ${Qt6_QMAKE_EXECUTABLE} DIRECTORY)
        
        # 查找 windeployqt 工具
        find_program(WINDEPLOYQT_EXECUTABLE 
            NAMES windeployqt windeployqt6
            HINTS ${Qt6_BIN_DIR}
        )
        
        if(WINDEPLOYQT_EXECUTABLE)
            # 添加自定义命令在编译后运行 windeployqt
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${WINDEPLOYQT_EXECUTABLE} 
                    --release 
                    \"${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.exe\"
                COMMENT "Deploying Qt dependencies with windeployqt..."
            )
            message(STATUS "windeployqt found: ${WINDEPLOYQT_EXECUTABLE}")
        else()
            message(WARNING "windeployqt not found. You may need to manually deploy Qt libraries.")
        endif()
    endif()
endif()

# 启用测试
enable_testing()

# 添加测试子目录
add_subdirectory(tests)
