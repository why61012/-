cmake_minimum_required(VERSION 3.20)
project(LibraryManagementSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(library_server src/main.cpp)

# 包含目录
target_include_directories(library_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 链接 socket 库（Windows 需要 ws2_32）
if(WIN32)
    target_link_libraries(library_server PRIVATE ws2_32)
endif()
