#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "base_common.hpp"
#include "base_arena.hpp"
#include "base_string.hpp"
#include "base_slice.hpp"

// @String =====================================================================

String alloc_str(u64 len, Arena *arena)
{
  String result;
  result.data = arena_push(arena, char, len);
  result.len = len;

  for (u64 i = 0; i < len; i++)
  {
    result.data[i] = '\0';
  }

  return result;
}

String String::clone(Arena *arena) const
{
  return String::copy(*this, arena);
}

String String::clone_from_cstr(cstr cstr, Arena *arena) const
{
  return String::copy_cstr(cstr, cstr_len(cstr), arena);
}

cstr String::clone_to_cstr(Arena *arena) const
{
  char *result = arena_push(arena, char, this->len+1);
  memcpy(result, this->data, this->len);
  result[this->len] = '\0';
  return (cstr) result;
}

bool String::equals(String s1, String s2)
{
  if (s1.len != s2.len) return false;

  bool result = true;

  for (u64 i = 0; i < s1.len; i++)
  {
    if (s1.data[i] != s2.data[i])
    {
      result = false;
      break;
    }
  }

  return result;
}

bool String::contains(String substr) const
{
  if (this->len < substr.len) return false;

  bool result = false;

  for (u64 i = 0; i < this->len-substr.len+1; i++)
  {
    if (this->data[i] == substr.data[0])
    {
      for (u64 j = 0; j < substr.len; j++)
      {
        if (this->data[i+j] != substr.data[j]) break;

        if (j == substr.len-1)
        {
          result = true;
          return result;
        }
      }
    }
  }

  return result;
}

i64 String::find(String substr, u64 start, u64 end) const
{
  if (this->len < substr.len || start >= this->len) return -1;

  i64 result = -1;

  for (u64 i = start; i < this->len-substr.len+1; i++)
  {
    if (this->data[i] == substr.data[0])
    {
      for (u64 j = 0; j < substr.len; j++)
      {
        if (this->data[i+j] != substr.data[j]) break;

        if (j == substr.len-1)
        {
          result = (i64) i;
          return result;
        }
      }
    }
  }

  return result;
}

i64 String::find(char c, u64 start, u64 end) const
{
  if (start >= this->len) return false;

  i64 result = -1;

  for (u64 i = start; i < this->len; i++)
  {
    if (this->data[i] == c)
    {
      result = (i64) i;
      break;
    }
  }

  return result;
}

String String::copy(String str, Arena *arena)
{
  String result;
  result.data = arena_push(arena, char, str.len+1);

  for (u64 i = 0; i < str.len; i++)
  {
    result.data[i] = str.data[i];
  }
  
  result.len = str.len;
  result.data[result.len] = '\0';

  return result;
}

String String::copy_into(String src, String *dest)
{
  for (u64 i = 0; i < src.len; i++)
  {
    dest->data[i] = src.data[i];
  }
  
  dest->len = src.len;

  return *dest;
}

String String::copy_cstr(cstr s, u64 len, Arena *arena)
{
  String result;
  result.data = arena_push(arena, char, len+1);

  for (u64 i = 0; i < len; i++)
  {
    result.data[i] = s[i];
  }

  result.len = len;
  result.data[result.len] = '\0';

  return result;
}

String String::insert_at(String substr, u64 loc, Arena *arena) const
{ 
  u64 len = this->len >= substr.len + loc ? this->len : substr.len + loc;
  String result = alloc_str(len, arena);

  u64 substr_idx = 0;
  for (u64 i = 0; i < len; i++)
  {
    if (i >= loc && i < substr.len + loc)
    {
      result.data[i] = substr.data[substr_idx];
      substr_idx++;
    }
    else
    {
      result.data[i] = this->data[i];
    }
  }

  return result;
}

String String::concat(String s1, String s2, Arena *arena)
{
  String result = alloc_str(s1.len + s2.len + 1, arena);
  result.len -= 1;

  for (u64 i = 0; i < s1.len; i++)
  {
    result.data[i] = s1.data[i];
  }

  for (u64 i = 0; i < s2.len; i++)
  {
    result.data[i+s1.len] = s2.data[i];
  }
  
  result.data[result.len] = '\0';

  return result;
}

String String::slice(u64 start, u64 end) const
{
  assert(start < end);
  assert(start >= 0);
  assert(end > 0);
  assert(end <= this->len);

  String result = {0};
  result.data = this->data + start;
  result.len = static_cast<u64>(end - start);

  return result;
}

String String::strip_front(String substr) const
{
  assert(substr.len <= this->len);

  String result = {0};

  u64 front_len = substr.len;
  String front = this->slice(0, front_len);
  if (String::equals(front, substr))
  {
    result = this->slice(front_len, this->len);
    result.len = this->len-front_len;
  }
  
  return result;
}

String String::strip_back(String substr) const
{
  assert(substr.len <= this->len);

  String result = {0};

  u64 back_len = this->len - substr.len;
  String back = this->slice(back_len, this->len);
  if (String::equals(back, substr))
  {
    result = this->slice(0, back_len);
    result.len = back_len;
  }
  
  return result;
}

String String::to_lower(Arena *arena) const
{
  String result = alloc_str(this->len, arena);

  for (u64 i = 0; i < this->len; i++)
  {
    if (this->data[i] >= 'A' && this->data[i] <= 'Z')
    {
      result.data[i] = this->data[i] + 32;
    }
    else
    {
      result.data[i] = this->data[i];
    }
  }

  return result;
}

String String::to_upper(Arena *arena) const
{
  String result = alloc_str(this->len, arena);

  for (u64 i = 0; i < this->len; i++)
  {
    if (this->data[i] >= 'a' && this->data[i] <= 'z')
    {
      result.data[i] = this->data[i] - 32;
    }
    else
    {
      result.data[i] = this->data[i];
    }
  }

  return result;
}

Slice<char> String::to_slice() const
{
  return Slice<char>((char *) this->data, this->len);
}

void String::print() const
{
  printf("%.*s\n", (u32) this->len, this->data);
}

// String_Builder ///////////////////////////////////////////////////////////////

void String_Builder::resize(u64 count)
{
  this->buffer.resize(count);
}

void String_Builder::write_char(char chr)
{
  this->buffer.append(chr);
}

void String_Builder::write_string(String str)
{
  for (u64 i = 0; i < str.len; i++)
  {
    this->buffer.append(str[i]);
  }
}

// void String_Builder::write_string(String str, ...)
// {
//   va_list vargs;
//   va_start(vargs, str);

//   Arena scratch(MiB(16));
  
//   u64 size = MiB(1);
//   String text = alloc_str(size - 8, &scratch);
//   text.len = (u64) vsnprintf(text.data, size, str.data, vargs);
//   for (u64 i = 0; i < text.len; i++)
//   {
//     this->buffer.append(text[i]);
//   }

//   scratch.release();
//   va_end(vargs);
// }

String String_Builder::to_string() const
{
  return str_from_slice(this->buffer.to_slice());
}

// cstr /////////////////////////////////////////////////////////////////////////

inline
u64 cstr_len(cstr s)
{
  u64 len = 0;
  for (; s[len]; len++);
  return len;
}

void copy_cstr_into_str(String *dest, cstr src)
{
  u64 len = cstr_len(src);
  for (u64 i = 0; i < len; i++)
  {
    dest->data[i] = src[i];
  }
  
  dest->len = len;
}

String str_from_cstr(cstr src)
{
  String result;

  u64 len = cstr_len(src);
  for (u64 i = 0; i < len; i++)
  {
    result.data[i] = src[i];
  }
  
  result.len = len;

  return result;
}

String str_from_slice(Slice<byte> slc)
{
  return {(char *) slc.data, slc.len};
}

String str_from_slice(Slice<char> slc)
{
  return {slc.data, slc.len};
}
