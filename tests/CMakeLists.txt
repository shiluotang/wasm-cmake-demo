file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES FALSE
    test_*.cpp
    test_*.cc
)

foreach(TEST_SOURCE IN LISTS TEST_SOURCES)
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WLE)
    add_executable(${TEST_NAME})
    target_sources(${TEST_NAME} PRIVATE ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PUBLIC foo)
    target_compile_options(${TEST_NAME} PRIVATE -s NO_DISABLE_EXCEPTION_CATCHING)
    target_link_options(${TEST_NAME} PRIVATE -s NO_DISABLE_EXCEPTION_CATCHING)
    target_link_options(${TEST_NAME} PRIVATE --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data/@/)
    add_test(NAME ${TEST_NAME}
        COMMAND
            /usr/bin/js
            --no-experimental-fetch
            --trace-uncaught
            $<TARGET_FILE:${TEST_NAME}>)
endforeach()
