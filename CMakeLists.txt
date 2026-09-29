cmake_minimum_required(VERSION 3.25)
project(Perekop LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PEREKOP_ENABLE_NATIVE_OPTIMIZATIONS "CPU native optimizations" OFF)

set(LIB "${CMAKE_SOURCE_DIR}/lib")
set(SRC "${CMAKE_SOURCE_DIR}/src")
set(INC "${CMAKE_SOURCE_DIR}/include")

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)

add_subdirectory("${LIB}/glfw")
add_subdirectory("${LIB}/freetype")

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS "${SRC}/*")

add_executable(Perekop
    ${SRCS}
    "${LIB}/glad/src/glad.c"
)

target_include_directories(Perekop PRIVATE
    "${INC}"
    "${LIB}"
    "${LIB}/glm"
    "${LIB}/glad/include"
)

target_link_libraries(Perekop PRIVATE
    glfw
    freetype
    OpenGL::GL
)

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