cmake_minimum_required(VERSION 3.0)
project(simple_protocol_lib)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/simple_protocol.cpp
)

add_library(simple_protocol_lib STATIC ${SOURCE_FILES})
