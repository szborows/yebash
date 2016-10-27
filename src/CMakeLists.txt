set(SOURCES
    yebash.cpp
    History.cpp
    KeyHandlers.cpp
    TerminalInfo.cpp
    HistorySuggestion.cpp
)

set(CMAKE_EXE_LINKER_FLAGS "-shared -fPIC ${CMAKE_EXE_LINKER_FLAGS}")

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} -ldl)
