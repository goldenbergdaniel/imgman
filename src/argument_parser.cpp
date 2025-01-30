#include <math.h>

#include "argument_parser.hpp"

Argument NIL_ARGUMENT = (Argument) {};

String string_from_argv(char *argv[], i32 argc, Arena *arena)
{
  String result;
  char *buf = arena_push(arena, char, MAX_ARGUMENT_STRING_LENGTH);

  i32 str_idx = 0;
  for (i32 arg_idx = 1; arg_idx < argc; arg_idx++)
  {
    for (i32 j = 0; j < (i32) cstr_len(argv[arg_idx]); j++)
    {
      buf[str_idx++] = argv[arg_idx][j];
    }

    if (arg_idx != argc - 1)
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
  Argument *arg_buf = arena_push(arena, Argument, MAX_ARGUMENT_COUNT);
  u32 arg_cnt = 0;

  // - Tokenize ---
  {
    u64 tokenizer_last_pos = 0;
    for (u64 tokenizer_pos = 0; tokenizer_pos < arg_str.len(); tokenizer_pos++)
    {
      char c = arg_str[tokenizer_pos];

      if (c == ' ')
      {
        String substr = arg_str.slice(tokenizer_last_pos, tokenizer_pos);
        Argument arg(substr);
        // arg.str.print();
        arg_buf[arg_cnt++] = arg;
        tokenizer_last_pos = tokenizer_pos + 1;
      }
      else if (c == ':' || c == ',')
      {
        String substr = arg_str.slice(tokenizer_last_pos, tokenizer_pos);
        Argument arg(substr);
        // arg.str.print();
        arg_buf[arg_cnt++] = arg;
        tokenizer_last_pos = tokenizer_pos + 1;
      }
      else if (tokenizer_pos == arg_str.len()-1)
      {
        String substr = arg_str.slice(tokenizer_last_pos, tokenizer_pos+1);
        Argument arg(substr);
        // arg.str.print();
        arg_buf[arg_cnt++] = arg;
      }
    }
  }

  result.from_ptr(arg_buf, arg_cnt);

  // - Classify ---
  {
    u32 cmd_idx = 0;
    u32 local_arg_idx = 0;

    for (u64 arg_idx = 0; arg_idx < result.len; arg_idx++)
    {
      Argument &arg = result[arg_idx];
      if (string_is_argument(arg.str))
      {
        arg.kind = Argument_Kind_COMMAND;
        arg.arg_pos = -1;
        arg.cmd_pos = ++cmd_idx;
        local_arg_idx = 0;
      }
      else if (string_is_number(arg.str))
      {
        arg.kind = Argument_Kind_NUMBER;
        arg.value.number = number_from_string(arg.str);
        arg.arg_pos = ++local_arg_idx;
        arg.cmd_pos = cmd_idx+1;
      }
      else if (string_is_bool(arg.str))
      {
        arg.kind = Argument_Kind_BOOLEAN;
        arg.value.boolean = bool_from_string(arg.str); 
        arg.arg_pos = ++local_arg_idx;
        arg.cmd_pos = cmd_idx+1;
      }
      else
      {
        arg.kind = Argument_Kind_FLAGS;
        arg.arg_pos = ++local_arg_idx;
        arg.cmd_pos = cmd_idx+1;
      }
    }
  }

  return result;
}

Argument *Argument_Store::consume()
{
  Argument *result = &NIL_ARGUMENT;
  if (this->arg_pos != this->data.len)
  {
    result = &this->data[this->arg_pos++];
  }

  return result;
}

bool string_is_argument(String str)
{
  return str[0] == '-';
}

bool string_is_number(String str)
{
  if (str.len() == 0) return false;
  if (str.len() > 1 && str[0] == '0') return false;

  bool is_negative = str.len() > 1 && str[0] == '-';
  u64 start = is_negative ? 1 : 0;
  for (u64 i = start; i < str.len(); i++)
  {
    if (str[i] < '0' || str[i] > '9') return false;
  }

  return true;
}

i64 number_from_string(String str)
{
  i64 result = 0;

  bool is_negative = str[0] == '-';
  String digits = str;
  if (is_negative)
  {
    digits = str.slice(1, str.len());
  }

  for (u64 i = 0; i < digits.len(); i++)
  {
    result += (i64) (digits[i] - 48) * (i64) (powf(10, (f32) (digits.len()-i-1)));
  }

  if (is_negative)
  {
    result *= -1;
  }

  return result;
}

bool string_is_bool(String str)
{
  return str == str("false") || str == str("true");
}

bool bool_from_string(String str)
{
  assert(string_is_bool(str));
  return str == str("false") ? false : true;
}
