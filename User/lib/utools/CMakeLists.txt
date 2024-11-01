cmake_minimum_required(VERSION 3.5)
project(utools_lib)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umempool
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coded
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logger
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utime
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ufmt
    ${CMAKE_CURRENT_SOURCE_DIR}/src/collab
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umath
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fliter
    ${CMAKE_CURRENT_SOURCE_DIR}/src/about_mang
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pattern
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umemory
)

# 查找所有源文件
file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umempool/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coded/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logger/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utime/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ufmt/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/collab/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umath/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fliter/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/about_mang/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pattern/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umemory/*.cpp
)

add_library(utools_lib STATIC ${SOURCE_FILES})
