cmake_minimum_required(VERSION 3.15)
project(RandomPhotoServer VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 源文件
set(SOURCES
    src/main.cpp
)

# 创建可执行文件
add_executable(photo_server ${SOURCES})

# Windows 平台特定设置
if(WIN32)
    target_link_libraries(photo_server ws2_32)
    
    # 设置控制台输出为 UTF-8
    if(MSVC)
        target_compile_options(photo_server PRIVATE /utf-8)
        set_target_properties(photo_server PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:CONSOLE"
        )
    else()
        # MinGW
        target_compile_options(photo_server PRIVATE -municode)
    endif()
endif()

# Linux/Unix 平台特定设置
if(UNIX)
    target_link_libraries(photo_server pthread)
    
    # 查找 OpenSSL (可选，用于 HTTPS 支持)
    find_package(OpenSSL)
    if(OPENSSL_FOUND)
        target_link_libraries(photo_server OpenSSL::SSL OpenSSL::Crypto)
        target_compile_definitions(photo_server PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
    endif()
endif()

# 编译选项
if(MSVC)
    target_compile_options(photo_server PRIVATE /W4)
else()
    target_compile_options(photo_server PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Release 优化选项
if(CMAKE_BUILD_TYPE MATCHES Release)
    if(MSVC)
        target_compile_options(photo_server PRIVATE /O2)
    else()
        target_compile_options(photo_server PRIVATE -O3)
    endif()
endif()

# 安装规则
install(TARGETS photo_server RUNTIME DESTINATION bin)

# 打印配置信息
message(STATUS "==================================")
message(STATUS "Random Photo Server Configuration")
message(STATUS "==================================")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "==================================")




