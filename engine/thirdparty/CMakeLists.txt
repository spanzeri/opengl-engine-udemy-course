cmake_minimum_required(VERSION 4.0)

# Third party dependencies

set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw-3.4)

add_subdirectory(fmt-12.1.0)
add_subdirectory(json-3.12.0)

set(GLM_ENABLE_CXX_20      ON CACHE BOOL "" ON)
set(GLM_ENABLE_SIMD_SSE4_2 ON CACHE BOOL "" ON)
add_subdirectory(glm-1.0.3)
target_compile_definitions(glm PUBLIC GLM_ENABLE_EXPERIMENTAL)

##
## Dependencies without a cmake file
##

# Glad

add_library(glad STATIC glad/src/gl.c glad/include/glad/gl.h)
target_include_directories(glad PUBLIC glad/include)

# Stb

add_library(stb INTERFACE)
file(GLOB_RECURSE STB_SOURCES CONFIGURE_DEPENDS "stb/*.h")
target_sources(stb PRIVATE ${STB_SOURCES})
target_include_directories(stb INTERFACE stb)

# CGLTF

add_library(cgltf INTERFACE)
target_sources(cgltf PRIVATE cgltf-1.15/cgltf.h)
target_include_directories(cgltf INTERFACE cgltf-1.15)

