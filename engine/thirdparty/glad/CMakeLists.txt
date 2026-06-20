cmake_minimum_required(VERSION 4.0)

add_library(glad STATIC
    src/gl.c
    include/glad/gl.h
)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

