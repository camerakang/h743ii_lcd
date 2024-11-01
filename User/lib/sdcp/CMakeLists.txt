cmake_minimum_required(VERSION 3.5)
project(simple_datagram_control_protocol_lib)

# 指定最小 C++ 标准为 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED FALSE) # 不强制要求

# 包含头文件目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 查找所有 cpp 文件并存储到 SOURCE_FILES 变量中
file(GLOB SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# 添加静态库
add_library(simple_datagram_control_protocol_lib STATIC ${SOURCE_FILES})

# 可选：如果有需要的话，设置库的输出目录
set_target_properties(simple_datagram_control_protocol_lib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
