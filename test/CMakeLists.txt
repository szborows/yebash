include_directories(../src)
set(SOURCES
    main.cpp
    helpers/Helpers.cpp
    EscapeCodeGenerator.cpp
    ANSIEscapeCodeGenerator.cpp
    ArrowHandler.cpp
    LineBuffer.cpp
    PrintBuffer.cpp
    History.cpp
    HistorySuggestion.cpp
    yebash.cpp)

add_executable(tests ${SOURCES})
target_link_libraries(tests yebash)

add_custom_target(tests-run DEPENDS tests COMMAND ./tests)

if (COVERAGE)
    add_custom_target(clean-coverage COMMAND find ${CMAKE_BINARY_DIR} -name '*.gcda' -exec rm {} \\\; )
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_custom_target(tests-cov
            DEPENDS tests
            COMMAND llvm-cov -version
            COMMAND llvm-profdata merge -version
            COMMAND LLVM_PROFILE_FILE=tests.profdata ./tests
            COMMAND llvm-profdata merge -instr tests.profdata -o merged.profdata
            COMMAND llvm-cov report ./tests -instr-profile=merged.profdata)
    else()
        add_custom_target(tests-cov
            DEPENDS clean-coverage tests-run
            COMMAND gcovr -r ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR} -e main.cpp)
        add_custom_target(tests-cov-html
            DEPENDS clean-coverage tests-run
            COMMAND gcovr -r ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR} -e main.cpp --html --html-details -o tests.html)
    endif()
endif()

add_custom_target(tests-valgrind
    DEPENDS tests
    COMMAND valgrind ./tests)

