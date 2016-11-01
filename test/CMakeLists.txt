include_directories(../src)
set(SOURCES
    main.cpp
    basic.cpp)

add_executable(tests ${SOURCES})
target_link_libraries(tests yebash)

add_custom_target(tests-run DEPENDS tests COMMAND ./tests)

