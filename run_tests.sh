#!/bin/bash

set -e

dir=$1
job=$2
export CXX=$COMPILER

mkdir -p build
cd build

cores=1

case "$job" in
    "valgrind")
        cmake $dir
        make tests-valgrind -j$cores ;;
    "coverage")
        cmake -DCOVERAGE=ON $dir
        make tests-cov -j$cores ;;
    *)
        cmake $dir
        make tests-run ;;
esac

