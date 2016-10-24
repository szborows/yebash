include_directories(../src)
set(SOURCES main.cpp)

target_link_libraries(tests yebash)
add_executable(tests ${SOURCES})
