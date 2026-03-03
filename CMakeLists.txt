cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(wasm-demo-cmake VERSION 1.0)

# find_package(Boost REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL sdl REQUIRED)
pkg_check_modules(EGL egl REQUIRED)
pkg_check_modules(GLES glesv2 REQUIRED)

add_subdirectory(src)
include(CTest)
add_subdirectory(tests)

