#pragma once

#include "base_common.hpp"

template <typename T>
class Slice
{
private:
  T *data = nullptr;
  u64 count = 0;

public:
  inline
  u64 len() const
  {
    return this->count;
  }

  inline 
  T* raw_data() const
  {
    return this->data;
  }

  inline 
  T &operator[](u64 index)
  {
    assert(index >= 0 && index < this->count);

    return this->data[index];
  }

  inline 
  void operator=(const Slice<T> &other)
  {
    this->data = other.data;
    this->count = other.count;
  }

  Slice<T> slice(u64 start, u64 end) const
  {
    assert(start < end && start >= 0 && end <= this->count);

    Slice<T> result;
    result.data = &this->data[start];
    result.count = end - start;

    return result;
  }

  void from_ptr(T *data, u64 count=1)
  {
    this->data = data;
    this->count = count;
  }

  void from_ptr(T *data, u64 start, u64 end)
  {
    assert(start < end && start >= 0);

    this->data = &data[start];
    this->count = end - start;
  }
};
