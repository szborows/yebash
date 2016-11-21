project(yebash CXX)
cmake_minimum_required(VERSION 3.0)

if (ASAN)
    set(ADDITIONAL_CXX_FLAGS "-fsanitize=address")
    set(ADDITIONAL_LD_FLAGS "-lasan")
endif()

if (COVERAGE)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(ADDITIONAL_CXX_FLAGS "${ADDITIONAL_CXX_FLAGS} -g -O0 -fprofile-instr-generate -fcoverage-mapping")
    else()
        set(ADDITIONAL_CXX_FLAGS "${ADDITIONAL_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
        set(ADDITIONAL_LD_FLAGS "${ADDITIONAL_LD_FLAGS} -lgcov")
    endif()
endif()

set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -Werror -Wextra ${ADDITIONAL_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${ADDITIONAL_LD_FLAGS}")

add_subdirectory(src)
add_subdirectory(test)

install(FILES bin/yebash DESTINATION bin PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
