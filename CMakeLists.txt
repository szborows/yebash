cmake_minimum_required(VERSION 3.0)

project(yebash C CXX)

add_definitions(-Wl,-shared -Wl,-fPIC)

add_library(${PROJECT_NAME} SHARED src/yebash.c)

target_link_libraries(${PROJECT_NAME} -ldl)

