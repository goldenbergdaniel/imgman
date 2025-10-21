#pragma once

#include "base_common.hpp"
#include "base_arena.hpp"
#include "base_dynamic_array.hpp"

// String //////////////////////////////////////////////////////////////////////

#define S(lit) ((String){.data = (char *) lit, .len = size_of(lit) - 1})

class String
{
public:
  char *data = nullptr;
  u64 len = 0;

  static bool equals(String s1, String s2);
  static String copy(String s, Arena *arena);
  static String copy_into(String src, String *dest);
  static String copy_cstr(cstr s, u64 len, Arena *arena);
  static String concat(String s1, String s2, Arena *arena);

  String clone(Arena *arena) const;
  String clone_from_cstr(cstr s, Arena *arena) const;
  cstr clone_to_cstr(Arena *arena) const;
  bool contains(String substr) const;
  i64 find(String substr, u64 start, u64 end) const;
  i64 find(char c, u64 start, u64 end) const;
  String slice(u64 start, u64 end) const;
  String insert_at(String substr, u64 loc, Arena *arena) const;
  String strip_front(String substr) const;
  String strip_back(String substr) const;
  String to_lower(Arena *arena) const;
  String to_upper(Arena *arena) const;
  Slice<char> to_slice() const;
  void print() const;

  inline const 
  char &operator[](u64 index) const
  {
    assert(index >= 0 && index < this->len);

    return this->data[index];
  }

  inline const 
  void operator=(String other)
  {
    this->data = other.data;
    this->len = other.len;
  }

  inline const 
  bool operator==(String other)
  {
    return equals(*this, other);
  }
};

String alloc_str(u64 len, Arena *arena);
String str_from_cstr(cstr src);
String str_from_slice(Slice<byte> slc);
String str_from_slice(Slice<char> slc);

// String_Builder //////////////////////////////////////////////////////////////

class String_Builder
{
public:
  Dynamic_Array<char> buffer;

  String_Builder(Arena *arena) : buffer(arena) {};

  void resize(u64 count);
  void write_char(char chr);
  void write_string(String str);
  // void write_string(String str, ...);
  String to_string() const;
};

// cstr ////////////////////////////////////////////////////////////////////////

u64 cstr_len(cstr s);
void cstr_copy(String *dest, cstr src);
