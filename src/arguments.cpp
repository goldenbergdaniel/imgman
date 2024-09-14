#include "arguments.hpp"

String string_from_argv(char *argv[], i32 argc, Arena *arena)
{
  String result;
  char *buf = arena_push(arena, char, MAX_ARGUMENT_LEN);

  i32 str_idx = 0;

  for (i32 i = 1; i < argc; i++)
  {
    for (i32 j = 0; j < cstr_len(argv[i]); j++)
    {
      buf[str_idx++] = argv[i][j];
    }

    if (i != argc - 1)
    {
      buf[str_idx++] = ' ';
    }
  }
  
  result = String(buf, str_idx);

  return result;
}

Slice<Argument> argument_list_from_string(String str, Arena *arena)
{
  Slice<Argument> result;

  

  return result;
}
