cmake_minimum_required(VERSION 3.25)
project(Perekop LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PEREKOP_ENABLE_NATIVE_OPTIMIZATIONS "CPU native optimizations" OFF)

set(ENGINE "${CMAKE_SOURCE_DIR}/engine")
set(GAME "${CMAKE_SOURCE_DIR}/game")

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)

add_subdirectory("${ENGINE}/lib/glfw")
add_subdirectory("${ENGINE}/lib/freetype")

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE ENG CONFIGURE_DEPENDS "${ENGINE}/src/*")
file(GLOB_RECURSE GAM CONFIGURE_DEPENDS "${GAME}/src/*")

set(SOURCES ${ENG} ${GAM})
list(FILTER SOURCES INCLUDE REGEX "\\.(cpp|c)$")

add_executable(Perekop
    ${SOURCES}
    "${ENGINE}/lib/glad/src/glad.c"
)

target_include_directories(Perekop PRIVATE
    "${ENGINE}/include"
    "${GAME}/include"
    "${ENGINE}/lib"
    "${ENGINE}/lib/glad/include"
    "${ENGINE}/lib/glm"
)

target_link_libraries(Perekop PRIVATE glfw freetype OpenGL::GL)

if(MSVC)
    target_compile_options(Perekop PRIVATE $<$<CONFIG:Debug>:/W4>)
else()
    target_compile_options(Perekop PRIVATE
        $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic>
        $<$<CONFIG:Release>:-O3 -funroll-loops>
    )
    if(PEREKOP_ENABLE_NATIVE_OPTIMIZATIONS)
        target_compile_options(Perekop PRIVATE $<$<CONFIG:Release>:-march=native>)
    endif()
endif()

set_target_properties(Perekop PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")