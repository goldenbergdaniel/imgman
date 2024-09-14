#pragma once

#include "base/base.hpp"

#define MAX_ARGUMENT_LEN 1024

enum ArgumentType
{

};

class Argument
{

};

String string_from_argv(char *argv[], i32 argc, Arena *arena);
Slice<Argument> arguments_from_string(String str, Arena *arena);
