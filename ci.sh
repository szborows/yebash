#!/bin/bash

set -e

docker pull mrokkk/devel
docker run -t -e "TERM=xterm-256color" -v $(pwd):/repo mrokkk/devel /bin/bash -c "/repo/run_tests.sh /repo $JOB"

