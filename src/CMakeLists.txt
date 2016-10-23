set(SOURCES
    yebash.cpp
    History.cpp
    KeyHandlers.cpp
    TerminalInfo.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} -ldl)
