cmake_minimum_required(VERSION 3.25)
project(Perekop LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENG  "${CMAKE_SOURCE_DIR}/Engine")
set(LIB  "${ENG}/lib")
set(SRC  "${ENG}/src")
set(INC  "${ENG}/include")
set(GSRC "${CMAKE_SOURCE_DIR}/src")

add_subdirectory("${LIB}/glfw")
add_subdirectory("${LIB}/freetype")

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS "${SRC}/*.cpp")
file(GLOB_RECURSE GSRCS CONFIGURE_DEPENDS "${GSRC}/*.cpp") 

add_executable(Perekop
    ${SRCS}
    ${GSRCS}
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
        $<$<CONFIG:Debug>:>
        $<$<CONFIG:Release>:-O3 -funroll-loops>
    )
endif()

set_target_properties(Perekop PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")