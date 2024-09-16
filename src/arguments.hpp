#pragma once

#include "base/base.hpp"

#define MAX_ARGUMENT_LEN 1024
#define MAX_ARGUMENTs 32

enum ArgumentType
{
  ArgumentType_NIL,

  ArgumentType_FLAG,
  ArgumentType_BOOLEAN,
  ArgumentType_NUMBER,
  ArgumentType_PATH,
};

union ArgumentValue
{
  bool boolean;
  u64 number;
};

struct Argument
{
  String str = 0;
  ArgumentValue value = ArgumentValue{};
  ArgumentType type = ArgumentType_NIL;

  Argument() {};

  Argument(String s)
  {
    this->str = s;
  }

  Argument(String s, ArgumentValue v, ArgumentType t)
  { 
    this->str = s; 
    this->value = v;
    this->type = t;
  }
};

String string_from_argv(char *argv[], i32 argc, Arena *arena);
Slice<Argument> arguments_from_string(String str, Arena *arena);
