project(yebash CXX)
cmake_minimum_required(VERSION 3.0)

if (ASAN)
    set(ADDITIONAL_CXX_FLAGS "-fsanitize=address")
    set(ADDITIONAL_LD_FLAGS "-lasan")
endif()

set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -Werror -Wextra ${ADDITIONAL_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-shared -fPIC ${ADDITIONAL_LD_FLAGS}")

add_subdirectory(src)
add_subdirectory(test)
