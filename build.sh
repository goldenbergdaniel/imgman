#!/bin/bash
set -e

CC=clang++

clang++ src/main.cpp -o Vsim -std=c++17 -O0 -g -Wall -Wno-unused -Wconversion -Werror -fno-exceptions
./Vsim "$@"
