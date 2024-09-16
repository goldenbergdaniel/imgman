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

Slice<Argument> arguments_from_string(String arg_str, Arena *arena)
{
  Slice<Argument> result;
  u32 argument_idx = 0;

  Argument *buf = arena_push(arena, Argument, 32);

  // Tokenize ----------------
  {
    String substr;
    i64 tokenizer_pos = 0;
    i64 last_pos = 0;
    
    for (;tokenizer_pos < arg_str.len(); tokenizer_pos++)
    {
      char c = arg_str[tokenizer_pos];
      if (c == ' ' || c == ':' || c == ',' ||
          tokenizer_pos == arg_str.len()-1)
      {
        substr = arg_str.slice(last_pos, tokenizer_pos);
        Argument arg = Argument(substr);
        buf[argument_idx++] = arg;
        last_pos = ++tokenizer_pos;
      }
    }
  }

  result.from_ptr(buf, argument_idx);

  return result;
}
