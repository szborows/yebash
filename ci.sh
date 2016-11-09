#!/bin/bash

set -e

job=$1
export CXX=$COMPILER

# TODO remove this after fixing yebash
if [ ! -f ~/.bash_history ]; then
    touch ~/.bash_history
fi

mkdir -p build
cd build

cores=$(nproc)

case "$job" in
    "valgrind")
        cmake ..
        make tests-valgrind -j$cores ;;
    "coverage")
        cmake .. -DCOVERAGE=ON
        make tests-cov -j$cores ;;
    *)
        cmake ..
        make tests-run ;;
esac

