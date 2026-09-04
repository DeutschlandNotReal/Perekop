cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER   C:/msys64/mingw64/bin/clang.exe   CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER C:/msys64/mingw64/bin/clang++.exe CACHE FILEPATH "" FORCE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

project(Perekop LANGUAGES C CXX)

set(ENGINE ${CMAKE_SOURCE_DIR}/engine)
set(GAME ${CMAKE_SOURCE_DIR}/game)

add_subdirectory(${ENGINE}/lib/glfw)
add_subdirectory(${ENGINE}/lib/freetype)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${ENGINE}/src/*.cpp
    ${GAME}/src/*.cpp
)

add_executable(perekop
    ${SOURCES}
    ${ENGINE}/lib/glad/src/glad.c
)

target_include_directories(perekop PRIVATE
    ${ENGINE}/lib/glad/include
    ${ENGINE}/lib/glm
    ${ENGINE}/lib
    ${ENGINE}/lib/freetype/include
    ${ENGINE}/include
    ${GAME}/include
)

target_compile_options(perekop PRIVATE
    $<$<CONFIG:Release>:-O3;-march=native;-funroll-loops>
    $<$<CONFIG:Debug>:-Wall;-Wextra;-Wpedantic>
)

target_link_libraries(perekop PRIVATE
    glfw
    freetype
    OpenGL::GL
)

set_target_properties(perekop PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)