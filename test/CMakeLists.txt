include_directories(../src)
set(SOURCES
    main.cpp
    EscapeCodeGenerator.cpp
    ANSIEscapeCodeGenerator.cpp
    ArrowHandler.cpp
    LineBuffer.cpp
    PrintBuffer.cpp
    basic.cpp)

add_executable(tests ${SOURCES})
target_link_libraries(tests yebash)

add_custom_target(tests-run DEPENDS tests COMMAND ./tests)

if (COVERAGE)
    add_custom_target(clean-coverage COMMAND find ${CMAKE_BINARY_DIR} -name '*.gcda' -exec rm {} \\\; )
    add_custom_target(tests-cov
        DEPENDS clean-coverage tests-run
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR} -e main.cpp)
    add_custom_target(tests-cov-html
        DEPENDS clean-coverage tests-run
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR} -e main.cpp --html --html-details -o tests.html)
endif()

add_custom_target(tests-valgrind
    DEPENDS tests
    COMMAND valgrind ./tests)

