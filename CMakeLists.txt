cmake_minimum_required(VERSION 3.10)
project(Perekop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(engine ${CMAKE_SOURCE_DIR}/pkengine)
set(game ${CMAKE_SOURCE_DIR}/game)

add_subdirectory(${engine}/lib/glfw)

include_directories(
    ${engine}/lib/glfw/include
    ${engine}/lib/glad/include
    ${engine}/lib/glm
    ${engine}/include
    ${game}/include
)

file(GLOB_RECURSE engine_src ${engine}/cli/*.cpp ${engine}/src/*.cpp)
file(GLOB_RECURSE game_src ${game}/src/*.cpp)

add_executable(perekop
    ${engine_src}
    ${game_src}
    ${engine}/lib/glad/src/glad.c
)

target_link_libraries(perekop
    glfw
    opengl32
)