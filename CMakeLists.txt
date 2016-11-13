project(yebash CXX)
cmake_minimum_required(VERSION 3.0)

if (ASAN)
    set(ADDITIONAL_CXX_FLAGS "-fsanitize=address")
    set(ADDITIONAL_LD_FLAGS "-lasan")
endif()

if (COVERAGE)
    set(ADDITIONAL_CXX_FLAGS "${ADDITIONAL_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    set(ADDITIONAL_LD_FLAGS "${ADDITIONAL_LD_FLAGS} -lgcov")
endif()

set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -Werror -Wextra ${ADDITIONAL_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${ADDITIONAL_LD_FLAGS}")

find_package(Boost COMPONENTS program_options) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_subdirectory(src)
    add_subdirectory(test)
else()
    message(FATAL "Boost.ProgramOptions not found")
endif()
