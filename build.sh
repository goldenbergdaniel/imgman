#!/bin/bash

mkdir -p out
g++ src/main.cpp -o out/imgman -std=c++23 -O0 -g -Wall -Werror -fwhole-program -fno-exceptions -Iext -Lext/stb -lstbi
