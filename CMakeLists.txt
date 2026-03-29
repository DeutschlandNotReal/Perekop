cmake_minimum_required(VERSION 3.10)
project(Perekop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIB_DIR ${CMAKE_SOURCE_DIR}/pkengine/lib)
set(GAME_DIR ${CMAKE_SOURCE_DIR}/game)

add_subdirectory(${LIB_DIR}/glfw)

include_directories(
    ${LIB_DIR}/glfw/include
    ${LIB_DIR}/glad/include
    ${LIB_DIR}/glm
    ${CMAKE_SOURCE_DIR}/pkengine/include
    ${GAME_DIR}/include
)

file(GLOB_RECURSE ENGINE_SRC ${CMAKE_SOURCE_DIR}/pkengine/src/*.cpp)
file(GLOB_RECURSE GAME_SRC ${GAME_DIR}/src/*.cpp)

add_executable(perekop
    ${ENGINE_SRC}
    ${GAME_SRC}
    ${LIB_DIR}/glad/src/glad.c
)

target_link_libraries(perekop
    glfw
    opengl32
)