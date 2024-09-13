#pragma once

#include "base_common.hpp"
#include "base_arena.hpp"

// @String =====================================================================

#define str(lit) ((String) {(char *) lit, size_of (lit) - 1})

class String
{
public:
  char *data = nullptr;
  u64 size = 0;

public:
  inline u64 len() const { return this->size; };
  inline char *raw_data() const { return this->data; };

  static bool equals(String s1, String s2);
  static String copy(String s, Arena *arena);
  static String copy_into(String src, String *dest);
  static String concat(String s1, String s2, Arena *arena);

  String clone(Arena *arena) const;
  String from_cstring(char *cstr, Arena *arena) const;
  bool contains(String substr) const;
  i64 find(String substr, u64 start, u64 end) const;
  i64 find(char c, u64 start, u64 end) const;
  String slice(u64 start, u64 end) const;
  String insert_at(String substr, u64 loc, Arena *arena) const;
  String strip_front(String substr) const;
  String strip_back(String substr) const;
  String to_lower(Arena *arena) const;
  String to_upper(Arena *arena) const;

  inline
  const char operator[](u64 index) const
  {
    assert(index >= 0 && index < this->size);

    return this->data[index];
  }

  inline
  const void operator=(String other)
  {
    this->data = other.data;
    this->size = other.size;
  }
};

String alloc_str(u64 len, Arena *arena);

// @CStr =======================================================================

u64 cstr_len(char *cstr);
void cstr_copy(String *dest, char *src);
