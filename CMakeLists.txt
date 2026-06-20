cmake_minimum_required(VERSION 4.0)
project(opengl-engine)

set(PRODUCTION_BUILD OFF CACHE BOOL "Make this a production build" FORCE)

set(CMAKE_CXX_STANDARD              23)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS   ON)
set(CMAKE_CXX_VISIBILITY_PRESET     hidden)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CONFIGURATION_TYPES "debug;release" CACHE STRING "" FORCE)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release CACHE STRING "" FORCE)
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>")

if (MSVC)
    string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif ()

function (configure_target target)
    if (MSVC)
        set_property(TARGET ${target} PROPERTY
            CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:debug>:Debug>"
        )
        target_compile_options(${target} PRIVATE
            /GR- /EHs-c- $<$<CONFIG:release>:/O2> $<$<CONFIG:release>:/Zi
            /UUNICODE /U_UNICODE
            /W4 /Wx
        )
        target_compile_options(${target} PRIVATE $<$<CONFIG:release>:/DEBUG>)
        target_compile_definitions(${target} PRIVATE
            _HAS_EXCEPTIONS=0
        )
    else ()
        target_compile_options(${target} PRIVATE
            -fno-rtti -fno-exceptions $<$<CONFIG:Release>:-O3> $<$<CONFIG:Release>:-g>
            -Wall -pedantic -Werror
        )

        target_link_options(${target} PRIVATE -static-libstdc++ -static-libgcc)

        get_target_property(_type ${target} TYPE)

        if (_type STREQUAL "EXECUTABLE" OR _type STREQUAL "SHARED_LIBRARY" OR _type STREQUAL "MODULE_LIBRARY")
            find_program(OBJCOPY_EXECUTABLE objcopy REQUIRED)
            find_program(STRIP_EXECUTABLE   strip   REQUIRED)

            add_custom_command(
                TARGET ${target}
                POST_BUILD

                # Extract debug info
                COMMAND ${OBJCOPY_EXECUTABLE}
                        --only-keep-debug
                        $<TARGET_FILE:${target}>
                        $<TARGET_FILE:${target}>.debug

                # Strip executable
                COMMAND ${STRIP_EXECUTABLE}
                        --strip-debug
                        $<TARGET_FILE:${target}>

                # Add debug link
                COMMAND ${OBJCOPY_EXECUTABLE}
                        --add-gnu-debuglink=$<TARGET_FILE:${target}>.debug
                        $<TARGET_FILE:${target}>

                VERBATIM
            )
        endif ()
    endif ()
endfunction ()

add_subdirectory(engine)


# Executable
file(GLOB_RECURSE PROJ_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/code/**.cpp")

add_executable(${CMAKE_PROJECT_NAME} WIN32)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE engine)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${PROJ_SOURCES})
configure_target(${CMAKE_PROJECT_NAME})

