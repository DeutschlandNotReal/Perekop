cmake_minimum_required(VERSION 3.25)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LLVM_ROOT "C:/Program Files/LLVM")
set(CMAKE_C_COMPILER   "${LLVM_ROOT}/bin/clang.exe"   CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER "${LLVM_ROOT}/bin/clang++.exe" CACHE FILEPATH "" FORCE)
set(CMAKE_LINKER       "${LLVM_ROOT}/bin/lld-link.exe" CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(Perekop LANGUAGES C CXX)

set(engine ${CMAKE_SOURCE_DIR}/engine)
set(game ${CMAKE_SOURCE_DIR}/game)

#add_subdirectory(${engine}/lib/glfw)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE src CONFIGURE_DEPENDS
    ${engine}/src/*.cpp
    ${game}/src/*.cpp
)

add_executable(perekop
    ${src}
    ${engine}/lib/glad/src/glad.c
)

target_compile_features(perekop PRIVATE cxx_std_23)

# put every compile flag under the sun
# might break something soon but it is super super super fast
# -ffast-math;
target_compile_options(perekop PRIVATE
    $<$<CONFIG:Release>:-O3;-march=native;-flto;-funroll-loops;>
)

set_target_properties(perekop PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_include_directories(perekop PRIVATE
    ${engine}/lib/glfw/include
    ${engine}/lib/glad/include
    ${engine}/lib/glm
    ${engine}/lib
    ${engine}/include
    ${game}/include
)

target_precompile_headers(perekop PRIVATE ${engine}/include/PK/pch.hpp)

target_link_libraries(perekop PRIVATE glfw OpenGL::GL)