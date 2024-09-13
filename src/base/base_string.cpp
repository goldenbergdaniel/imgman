#include "base_common.hpp"
#include "base_arena.hpp"
#include "base_string.hpp"

// @String =====================================================================

String alloc_str(u64 len, Arena *arena)
{
  String result;
  result.data = arena_push(arena, char, len);
  result.size = len;

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

inline
String String::from_cstring(char *cstr, Arena *arena) const
{
  return String::copy((String) {cstr, cstr_len(cstr)-1}, arena);
}

bool String::equals(String s1, String s2)
{
  if (s1.len() != s2.len()) return false;

  bool result = true;

  for (u64 i = 0; i < s1.len(); i++)
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
  if (this->size < substr.len()) return false;

  bool result = false;

  for (u64 i = 0; i < this->size-substr.len()+1; i++)
  {
    if (this->data[i] == substr.data[0])
    {
      for (u64 j = 0; j < substr.len(); j++)
      {
        if (this->data[i+j] != substr.data[j]) break;

        if (j == substr.len()-1)
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
  if (this->size < substr.len() || start >= this->size) return -1;

  i64 result = -1;

  for (u64 i = start; i < this->size-substr.len()+1; i++)
  {
    if (this->data[i] == substr.data[0])
    {
      for (u64 j = 0; j < substr.len(); j++)
      {
        if (this->data[i+j] != substr.data[j]) break;

        if (j == substr.len()-1)
        {
          result = i;
          return result;
        }
      }
    }
  }

  return result;
}

i64 String::find(char c, u64 start, u64 end) const
{
  if (start >= this->size) return false;

  i64 result = -1;

  for (u64 i = start; i < this->size; i++)
  {
    if (this->data[i] == c)
    {
      result = i;
      break;
    }
  }

  return result;
}

String String::copy(String str, Arena *arena)
{
  String result = {0};
  result.data = arena_push(arena, char, str.len()+1);

  for (u64 i = 0; i < str.len(); i++)
  {
    result.data[i] = str.data[i];
  }
  
  result.size = str.len();
  result.data[result.len()] = '\0';

  return result;
}

String String::copy_into(String src, String *dest)
{
  for (u64 i = 0; i < src.len(); i++)
  {
    dest->data[i] = src.data[i];
  }
  
  dest->size = src.len();

  return *dest;
}

String String::insert_at(String substr, u64 loc, Arena *arena) const
{ 
  u64 len = this->size >= substr.len() + loc ? this->size : substr.len() + loc;
  String result = alloc_str(len, arena);

  u64 substr_idx = 0;
  for (u64 i = 0; i < len; i++)
  {
    if (i >= loc && i < substr.len() + loc)
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
  String result = alloc_str(s1.len() + s2.len() + 1, arena);
  result.size -= 1;

  for (u64 i = 0; i < s1.len(); i++)
  {
    result.data[i] = s1.data[i];
  }

  for (u64 i = 0; i < s2.len(); i++)
  {
    result.data[i+s1.len()] = s2.data[i];
  }
  
  result.data[result.len()] = '\0';

  return result;
}

String String::slice(u64 start, u64 end) const
{
  assert(start >= 0 && 
         start < this->size && 
         end > 0 && end <= this->size && 
         start < end);

  String result = {0};
  result.data = this->data + start;
  result.size = end - start;

  return result;
}

String String::strip_front(String substr) const
{
  assert(substr.len() <= this->size);

  String result = {0};

  u64 front_len = substr.len();
  String front = this->slice(0, front_len);
  if (String::equals(front, substr))
  {
    result = this->slice(front_len, this->size);
    result.size = this->size-front_len;
  }
  
  return result;
}

String String::strip_back(String substr) const
{
  assert(substr.len() <= this->size);

  String result = {0};

  u64 back_len = this->size - substr.len();
  String back = this->slice(back_len, this->size);
  if (String::equals(back, substr))
  {
    result = this->slice(0, back_len);
    result.size = back_len;
  }
  
  return result;
}

String String::to_lower(Arena *arena) const
{
  String result = alloc_str(this->size, arena);

  for (u64 i = 0; i < this->size; i++)
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
  String result = alloc_str(this->size, arena);

  for (u64 i = 0; i < this->size; i++)
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

// @CString ====================================================================

inline
u64 cstr_len(char *s)
{
  u64 len = 0;
  for (; s[len]; len++);
  return len+1;
}

void copy_cstr_into_str(String *dest, char *src)
{
  u64 len = cstr_len(src)-1;
  for (u64 i = 0; i < len; i++)
  {
    dest->data[i] = src[i];
  }
  
  dest->size = len;
}
