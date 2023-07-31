#!/bin/bash

set -e

mkdir -p build
cd build

em++ ../haste-proto/*.cpp

ls -al