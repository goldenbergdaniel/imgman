#pragma once

#include "globals.hpp"

template <typename T>
class Slice
{
private:
  T *data = nullptr;
  u64 size = 0;

public:
  inline
  u64 len() const
  {
    return this->size;
  }

  inline 
  T* raw_data() const
  {
    return this->data;
  }

  inline 
  T &operator[](u64 index)
  {
    assert(index >= 0 && index < this->size);

    return this->data[index];
  }

  inline 
  void operator=(const Slice<T> &other)
  {
    this->data = other.data;
    this->size = other.size;
  }

  Slice<T> slice(u64 start, u64 end) const
  {
    assert(start < end && start >= 0 && end <= this->size);

    Slice<T> result;
    result.data = &this->data[start];
    result.size = end - start;

    return result;
  }

  void from_ptr(T *data, u64 size)
  {
    this->data = data;
    this->size = size;
  }

  void from_ptr(T *data, u64 start, u64 end)
  {
    assert(start < end && start >= 0);

    this->data = &data[start];
    this->size = end - start;
  }
};
