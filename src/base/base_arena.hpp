#pragma once

#include "base_common.hpp"

#define KiB(bytes) ((u64) bytes << 10)
#define MiB(bytes) ((u64) bytes << 20)
#define GiB(bytes) ((u64) bytes << 30)

// @Arena ////////////////////////////////////////////////////////////////////////////////

class Arena
{
public:
  u64 size;
  byte *memory;
  byte *allocated;
  byte *committed;
  u8 id;
  bool decommit_on_clear;
  
  byte *push(u64 size, u64 align);
  void pop(u64 size);
  void clear();
};

#define arena_push(arena, T, count) (T *) arena->push(size_of(T) * count, align_of(T))

void init_scratch_arenas(void);
Arena get_scratch_arena(Arena *conflict);
