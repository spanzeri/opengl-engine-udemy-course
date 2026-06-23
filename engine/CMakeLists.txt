cmake_minimum_required(VERSION 4.0)
project(engine)

# Dependencies

set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw-3.4)

add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/fmt-12.1.0)
add_subdirectory(thirdparty/stb)
add_subdirectory(thirdparty/json-3.12.0)

set(GLM_ENABLE_CXX_20      ON CACHE BOOL "" ON)
set(GLM_ENABLE_SIMD_SSE4_2 ON CACHE BOOL "" ON)
add_subdirectory(thirdparty/glm-1.0.3)
target_compile_definitions(glm PUBLIC GLM_ENABLE_EXPERIMENTAL)

# Library

add_library(engine STATIC)

file(GLOB_RECURSE PROJ_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/code/**.cpp")
target_sources(engine PRIVATE ${PROJ_SOURCES})

target_link_libraries(engine
    PRIVATE
        glfw
        glad
        stb::stb
        nlohmann_json::nlohmann_json
    PUBLIC
        fmt::fmt
        glm::glm
)

configure_target(engine)

target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/code")
target_compile_definitions(engine PUBLIC "GLFW_INCLUDE_NONE=1" "JSON_NOEXCEPTION=1")

if (PRODUCTION_BUILD)
    target_compile_definitions(engine PUBLIC PRODUCTION_BUILD=1)
    target_compile_definitions(engine PUBLIC RESOURCE_PATH="./assets")
    target_compile_definitions(engine PUBLIC
        $<$<CONFIG:release>:CONFIG_RELEASE=1>
        $<$<CONFIG:debug>:CONFIG_DEBUG=1>
    )
else ()
    target_compile_definitions(engine PUBLIC PRODUCTION_BUILD=0)
    target_compile_definitions(engine PUBLIC RESOURCE_PATH="${CMAKE_SOURCE_DIR}/assets")
    target_compile_definitions(engine PUBLIC
        $<$<CONFIG:release>:CONFIG_RELEASE=1>
        $<$<CONFIG:debug>:CONFIG_DEBUG=1>
    )
endif()

