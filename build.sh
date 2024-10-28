#!/bin/bash

mkdir -p out
clang++ src/main.cpp -o out/imgman -std=c++17 -O0 -Iext -Lext/stb -lstbi
