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

struct Argument
{
  String str;

  virtual ArgumentType type() const;
};

struct FlagArgument
{
  ArgumentType type() const { return ArgumentType_FLAG; }
};

struct BooleanArgument : public Argument
{
  bool value;

  ArgumentType type() const { return ArgumentType_BOOLEAN; }
};

struct NumberArgument : public Argument
{
  u64 value;

  ArgumentType type() const { return ArgumentType_NUMBER; }
};

struct PathArgument : public Argument
{
  ArgumentType type() const { return ArgumentType_PATH; }
};

String string_from_argv(char *argv[], i32 argc, Arena *arena);
Slice<Argument> arguments_from_string(String str, Arena *arena);
