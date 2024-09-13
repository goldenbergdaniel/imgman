@echo off
setlocal

set SRC=src\main.cpp
set OUT=out\imgman.exe

set MODE=dev
if "%1%"=="d" set MODE=dbg
if "%1%"=="r" set MODE=rls

set COMMON= /std:c++17 /nologo /Iext

if "%MODE%"=="dev" set CFLAGS= /Od
if "%MODE%"=="dbg" set CFLAGS= /Od /Z7 /W1
if "%MODE%"=="rls" set CFLAGS= /O2

set LFLAGS= /link /incremental:no /libpath:ext stb\stbi.lib

cl %COMMON% %CFLAGS% %SRC% /Fe%OUT% %LFLAGS% || exit /b 1
