cmake_minimum_required(VERSION 4.0)
project(engine)

add_subdirectory(thirdparty)

# Library

add_library(engine STATIC)

file(GLOB_RECURSE PROJ_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/code/**.cpp")
target_sources(engine PRIVATE ${PROJ_SOURCES})

target_link_libraries(engine
    PRIVATE
        glfw
        glad
        stb
        nlohmann_json::nlohmann_json
        cgltf
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

