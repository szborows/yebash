#!/bin/bash

set -e

docker run -t -e "TERM=xterm-256color" -v $(pwd):/repo $IMAGE /bin/bash -c "CODECOV=$CODECOV CXX=$COMPILER /repo/run_tests.sh /repo $JOB"

