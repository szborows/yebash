#!/bin/bash

set -e

dir=$(dirname $0)

cd $dir
mkdir -p build
cd build

cores=$(nproc)

case "$JOB" in
    "valgrind")
        cmake ..
        make tests-valgrind -j$cores ;;
    "coverage")
        cmake -DCOVERAGE=ON ..
        make tests-cov -j$cores ;;
    *)
        cmake ..
        make tests-run -j$cores ;;
esac

