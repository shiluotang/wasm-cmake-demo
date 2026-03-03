add_library(foo STATIC)
target_sources(foo PRIVATE foo.cc)

# TODO FIXME handle installed header files
target_include_directories(foo PUBLIC ${CMAKE_SOURCE_DIR}/include)
