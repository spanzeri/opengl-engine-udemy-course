add_library(stb INTERFACE)
file(GLOB_RECURSE STB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/**.h")
target_sources(stb PRIVATE ${STB_SOURCES})
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(stb::stb ALIAS stb)

