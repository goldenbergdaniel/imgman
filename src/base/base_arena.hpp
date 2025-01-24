#pragma once

#include "base_common.hpp"

#define KiB(bytes) ((u64) bytes << 10)
#define MiB(bytes) ((u64) bytes << 20)
#define GiB(bytes) ((u64) bytes << 30)

// @Arena //////////////////////////////////////////////////////////////////////

class Arena
{
public:
  u64 size = 0;
  byte *memory = 0;
  byte *allocated = 0;
  byte *committed = 0;
  bool decommit_on_clear = false;
  u8 id = 0;

  Arena() {}
  Arena(u64 size);
  ~Arena();
  
  byte *push(u64 size, u64 align);
  void pop(u64 size);
  void clear();
  void release();
};

class Arena_Temp
{
public:
  Arena *data;
  byte *start;

  Arena_Temp() {}
  Arena_Temp(Arena *arena);
  ~Arena_Temp();
};

#define arena_push(arena, T, count) (T *) arena->push(size_of(T) * count, align_of(T))

Arena_Temp temp_begin(Arena *arena);
void temp_end(Arena_Temp temp);

void init_scratches(void);
Arena *get_scratch(Arena *conflict);
