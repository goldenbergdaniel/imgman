#pragma once

#include <stdio.h>
#include <string.h>

#include "base_common.hpp"

template <typename T>
class Slice
{
public:
  T *data = nullptr;
  u64 len = 0;

  Slice() {}

  Slice(cstr s)
  {
    this->from_ptr(s, strlen(s));
  }

  Slice(T *data, u64 count=1)
  {
    this->from_ptr(data, count);
  }

  Slice(T *data, u64 start, u64 end)
  {
    this->from_ptr(data, start, end);
  }

  inline 
  T &operator[](u64 index) const
  {
    assert(index >= 0 && index < this->len);

    return this->data[index];
  }

  inline 
  void operator=(const Slice<T> &other)
  {
    this->data = other.data;
    this->len = other.len;
  }

  inline
  bool operator==(const Slice<T> &other) const
  {
    if (this->len != other.len) return false;

    for (i32 i = 0; i < this->len; i++)
    {
      if (this->data[i] != other.data[i]) return false;
    }

    return true;
  }

  void from_ptr(T *data, u64 count=1)
  {
    assert(count > 0);

    this->data = data;
    this->len = count;
  }

  void from_ptr(T *data, u64 start, u64 end)
  {
    assert(start < end && start >= 0);

    this->data = &data[start];
    this->len = end - start;
  }

  Slice<T> slice(u64 start, u64 end) const
  {
    assert(start < end && start >= 0 && end <= this->len);

    Slice<T> result;
    result.data = &this->data[start];
    result.len = end - start;

    return result;
  }

  void append_at(Slice<T> slc, u64 pos)
  {
    assert(pos >= 0);

    for (u64 i = 0; i < slc.len; i++)
    {
      this->data[pos + i] = slc[i];
    }

    this->len += slc.len;
  }

  cstr clone_to_cstr()
  {
    T *data = new T[this->len+1];
    Slice<T> result = Slice<T>(data, this->len+1);
    result[result.len] = 0;
    return result.data;
  }

  void print(bool new_line) const
  {
    for (u64 i = 0; i < this->len; i++)
    {
      printf("%c", this->data[i]);
    }

    if (new_line) printf("\n");
  }
};
