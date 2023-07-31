#!/bin/bash

set -e

mkdir -p build
cd build

em++ ../haste-proto/*.cpp -s DISABLE_EXCEPTION_CATCHING=1 -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1 -Os

cp ../index.html .

ls -al