#pragma once

#include "base/base.hpp"

enum Channel
{
  Channel_R = 1 << 0,
  Channel_G = 1 << 1,
  Channel_B = 1 << 2,
};

enum Format_Kind
{
  Format_Kind_NIL,
  Format_Kind_PNG,
  Format_Kind_JPG,
  Format_Kind_TGA,
  Format_Kind_BMP,
};

typedef b64 Channel_Set;

class Image
{
public:
  byte *data = nullptr;
  i32 width = 0;
  i32 height = 0;
  i32 channels = 0;
  u64 size;
  Format_Kind format_kind = Format_Kind_NIL;

  void scale_color(f32 x, Channel_Set ch);
  void multiply_by_image(Image *other, Channel_Set ch);
  void screen_by_image(Image *other, Channel_Set ch);
  void add(i16 x, Channel_Set ch);
  void subtract_by_image(Image *other, Channel_Set ch);
  void overlay_by_image(Image *other, Channel_Set ch);
  void rotate();
  void resize(i32 width, i32 height);
  i32 compare_to_image(Image *other) const;

  void read_from_file(String path, Arena *arena);
  void read_from_image(Image *other, Arena *arena);
  void write_to_file(String path) const;
  void write_to_image(Image *other, Arena *arena) const;

  void print_stats() const;
};
