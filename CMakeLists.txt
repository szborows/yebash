project(yebash CXX)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -Werror -Wextra")
set(CMAKE_EXE_LINKER_FLAGS "-shared -fPIC")

add_subdirectory(src)
