#!/bin/bash

set -e

dir=$1
job=$2

mkdir -p build
cd build

cores=$(nproc)

case "$job" in
    "valgrind")
        cmake $dir
        make tests-valgrind -j$cores ;;
    "coverage")
        cmake -DCOVERAGE=ON $dir
        make tests-cov -j$cores ;;
    *)
        cmake $dir
        make tests-run -j$cores ;;
esac

set -x

if [ $CODECOV ]; then
    codecov --root $dir --gcov-root . --required
fi

