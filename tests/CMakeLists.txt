file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES FALSE
    test_*.cpp
    test_*.cc
)

foreach(TEST_SOURCE IN LISTS TEST_SOURCES)
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WLE)
    add_executable(${TEST_NAME})
    target_sources(${TEST_NAME} PRIVATE ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PUBLIC foo)
    # target_link_libraries(${TEST_NAME} PUBLIC SDL2::SDL2)
    # target_link_libraries(${TEST_NAME} PUBLIC OpenGL::EGL OpenGL::GLES)
    target_compile_options(${TEST_NAME} PRIVATE -s NO_DISABLE_EXCEPTION_CATCHING)
    target_link_options(${TEST_NAME} PRIVATE -s NO_DISABLE_EXCEPTION_CATCHING)
    target_link_options(${TEST_NAME} PRIVATE "-sLEGACY_GL_EMULATION=1")
    target_link_options(${TEST_NAME} PRIVATE "-sUSE_GLFW=2")
    target_link_options(${TEST_NAME} PRIVATE "-sERROR_ON_UNDEFINED_SYMBOLS=0")
    target_link_options(${TEST_NAME} PRIVATE "-sEXIT_RUNTIME=1")
    target_link_options(${TEST_NAME} PRIVATE --emrun)
    target_link_options(${TEST_NAME} PRIVATE --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data/@/)
    # add_test(NAME ${TEST_NAME}
    #     COMMAND
    #         node
    #         --no-experimental-fetch
    #         --trace-uncaught
    #         $<TARGET_FILE:${TEST_NAME}>)
    add_test(NAME ${TEST_NAME}
        COMMAND
            emrun $<TARGET_FILE:${TEST_NAME}>)
endforeach()
