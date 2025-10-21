#pragma once

#include "base_common.hpp"
#include "base_arena.hpp"
#include "base_slice.hpp"

#ifndef DYNMAIC_ARRAY_GROWTH_RATE
#define DYNAMIC_ARRAY_GROWTH_RATE 2
#endif

template <typename T>
class Dynamic_Array
{
public:
  T *data = nullptr;
  u64 len = 0;
  u64 cap = 0;
  Arena *arena = nullptr;

  Dynamic_Array(Arena *arena)
  {
    this->arena = arena;
  }

  inline 
  T &operator[](u64 idx) const
  {
    assert(idx >= 0 && idx < this->len);
    return this->data[idx];
  }

  inline 
  void operator=(const Slice<T> &other)
  {
    this->data = other.data;
    this->len = other.len;
    this->cap = other.cap;
  }

  inline
  Slice<T> slice(u64 start, u64 end) const
  {
    return Slice<T>(this->data, start, end);
  }

  inline
  Slice<T> to_slice() const
  {
    return Slice<T>(this->data, this->len);
  }

  void append(T elem)
  {
    if (this->len >= this->cap)
    {
      u64 new_cap = (u64) ((f32) this->cap * DYNAMIC_ARRAY_GROWTH_RATE + 1);
      this->resize(new_cap);
    }

    this->data[this->len] = elem;
    this->len += 1;
  }

  void resize(u64 count)
  {
    if (count > this->cap)
    {
      T *new_data = arena_push(this->arena, T, count);
      memcpy(new_data, this->data, this->len * size_of(T));
      this->data = new_data;
      this->cap = count;
    }
  }

  void clear()
  {
    memset(this->data, 0, this->cap * size_of(T)); 
  }
};
