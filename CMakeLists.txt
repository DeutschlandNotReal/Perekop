cmake_minimum_required(VERSION 3.25)
project(Perekop LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(engine ${CMAKE_SOURCE_DIR}/pkengine)
set(game ${CMAKE_SOURCE_DIR}/game)

add_subdirectory(${engine}/lib/glfw)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE engine_src CONFIGURE_DEPENDS
    ${engine}/cli/*.cpp
    ${engine}/src/*.cpp
)

file(GLOB_RECURSE game_src CONFIGURE_DEPENDS
    ${game}/src/*.cpp
)

add_executable(perekop
    ${engine_src}
    ${game_src}
    ${engine}/lib/glad/src/glad.c
)

target_compile_features(perekop PRIVATE cxx_std_20)

target_include_directories(perekop PRIVATE
    ${engine}/lib/glfw/include
    ${engine}/lib/glad/include
    ${engine}/lib/glm
    ${engine}/include
    ${game}/include
)

target_link_libraries(perekop PRIVATE glfw OpenGL::GL)