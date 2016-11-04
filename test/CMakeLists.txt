include_directories(../src)
set(SOURCES
    main.cpp
    ANSIEscapeCodeGenerator.cpp
    basic.cpp)

add_executable(tests ${SOURCES})
target_link_libraries(tests yebash)

add_custom_target(tests-run DEPENDS tests COMMAND ./tests)

if (COVERAGE)
    add_custom_target(clean-coverage COMMAND find ${CMAKE_BINARY_DIR} -name '*.gcda' -exec rm {} \\\; )
    add_custom_target(tests-cov
        DEPENDS clean-coverage tests-run
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR})
endif()

