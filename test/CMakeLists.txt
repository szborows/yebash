include_directories(../src)
set(SOURCES
    main.cpp
    basic.cpp)

add_executable(tests ${SOURCES})
target_link_libraries(tests yebash)
