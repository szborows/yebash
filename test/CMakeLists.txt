include_directories(../src)
set(SOURCES
    main.cpp
    basic.cpp)

add_executable(tests ${SOURCES})
target_link_libraries(tests yebash)

add_custom_target(tests-run DEPENDS tests COMMAND ./tests)

if (COVERAGE)
    add_custom_target(clean-coverage COMMAND find .. -name '*.gcda' -exec rm {} \\\; )
    add_custom_target(tests-cov
        DEPENDS clean-coverage tests-run
        COMMAND gcovr -r ../../src ..
        COMMAND gcovr --html --html-details -o coverage.html -r ../../src ..)
endif()

