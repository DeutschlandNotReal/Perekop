cmake_minimum_required(VERSION 3.10)
project(Perekop)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/pkengine/lib)
set(GLFW_INCLUDE ${LIB_DIR}/glfw/glfw-3.4.bin.WIN64/include)
set(GLEW_INCLUDE ${LIB_DIR}/glew/glew-2.2.0/include)
set(GLM_INCLUDE  ${LIB_DIR}/glm)
set(GLAD_INCLUDE ${LIB_DIR}/glad/include)
set(GLFW_LIB ${LIB_DIR}/glfw/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.lib)
set(GLEW_LIB ${LIB_DIR}/glew/glew-2.2.0/lib/Release/x64/glew32.lib)
file(GLOB_RECURSE SRC_FILES pkengine/src/*.cpp game/src/*.cpp)
list(APPEND SRC_FILES ${LIB_DIR}/glad/src/glad.c)
add_executable(pkengine ${SRC_FILES})
target_include_directories(pkengine PRIVATE
    ${GLFW_INCLUDE}
    ${GLEW_INCLUDE}
    ${GLM_INCLUDE}
    ${GLAD_INCLUDE}
    ${CMAKE_SOURCE_DIR}/pkengine/include
    ${CMAKE_SOURCE_DIR}/game/include
)
target_link_libraries(pkengine PRIVATE
    ${GLFW_LIB}
    ${GLEW_LIB}
    opengl32
)