cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ATTENTION project() command invoke toolchain settings
project(wasm-demo-cmake VERSION 1.0)

# override toolchain settings
set(CMAKE_EXECUTABLE_SUFFIX .html)

# find_package(Boost REQUIRED)
if (LINUX)
find_package(PkgConfig REQUIRED)

pkg_check_modules(EGL egl REQUIRED)
pkg_check_modules(GLES glesv2 REQUIRED)
endif()
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)

add_subdirectory(src)
include(CTest)
add_subdirectory(tests)

