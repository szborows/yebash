set(SOURCES
    main.cpp
    yebash.cpp
    History.cpp
    HistorySuggestion.cpp
    Printer.cpp
)

set(CMAKE_EXE_LINKER_FLAGS "-shared -fPIC ${CMAKE_EXE_LINKER_FLAGS}")

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} -ldl)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
