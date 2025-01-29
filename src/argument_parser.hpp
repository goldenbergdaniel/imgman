#pragma once

#include "base/base.hpp"

#define MAX_ARGUMENT_STRING_LENGTH 1024
#define MAX_ARGUMENT_COUNT 32

enum Argument_Kind
{
  Argument_Kind_NIL,

  Argument_Kind_COMMAND,
  Argument_Kind_BOOLEAN,
  Argument_Kind_NUMBER,
  Argument_Kind_PATH,
  Argument_Kind_CHANNELS,
};

union Argument_Value
{
  bool boolean;
  u64  number;
};

struct Argument
{
  String         str = {};
  Argument_Value value = {};
  Argument_Kind  kind = Argument_Kind_NIL;
  i32            arg_idx;
  i32            cmd_idx;
};

String string_from_argv(char *argv[], i32 argc, Arena *arena);
Slice<Argument> arguments_from_string(String str, Arena *arena);

bool string_is_argument(String str);
bool string_is_number(String str);
i64 number_from_string(String str);
bool string_is_bool(String str);
bool bool_from_string(String str);
Slice<char> flags_from_string(String str, String flags);
