cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_COMPILER_WORKS 1 CACHE BOOL "" FORCE)
set(CMAKE_C_COMPILER_WORKS 1 CACHE BOOL "" FORCE)
project(Perekop)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/pkengine/lib)
set(GLFW_INCLUDE ${LIB_DIR}/glfw/glfw-3.4.bin.WIN64/include)
set(GLEW_INCLUDE ${LIB_DIR}/glew/glew-2.2.0/include)
set(GLM_INCLUDE  ${LIB_DIR}/glm)
set(GLAD_INCLUDE ${LIB_DIR}/glad/include)
set(GLFW_LIB ${LIB_DIR}/glfw/glfw-3.4.bin.WIN64/lib-mingw-w64/libglfw3.a)
set(GLEW_LIB ${LIB_DIR}/glew/glew-2.2.0/lib/Release/x64/glew32.lib)
file(GLOB_RECURSE SRC_FILES pkengine/src/*.cpp game/src/*.cpp)
list(APPEND SRC_FILES ${LIB_DIR}/glad/src/glad.c)
add_executable(pkengine ${SRC_FILES})
target_include_directories(pkengine PRIVATE
    pkengine/include
    game/include
    ${GLFW_INCLUDE}
    ${GLEW_INCLUDE}
    ${GLM_INCLUDE}
    ${GLAD_INCLUDE}
)
target_precompile_headers(pkengine PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/pkengine/include/gl.hpp>"
    "$<$<COMPILE_LANGUAGE:CXX>:<vector$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<functional$<ANGLE-R>>"
)
target_link_libraries(pkengine PRIVATE
    ${GLFW_LIB}
    ${GLEW_LIB}
    opengl32
)