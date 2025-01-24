#pragma once

#include "base/base.hpp"

enum Channel 
{
  R = 1 << 0,
  G = 1 << 1,
  B = 1 << 2,
};

enum Format_Type
{
  NIL,

  PNG,
  JPG,
  TGA,
  BMP,
};

typedef b64 Channel_Set;

class Image
{
private:
  byte *data = nullptr;
  Format_Type type = Format_Type::NIL;

public:
  i32 width = 0;
  i32 height = 0;
  i32 channels = 0;

  u64 size() const { return this->width * this->height; }
  byte *raw_data() const { return this->data; }
  Format_Type format_type() const { return this->type; }

  void scale_color(f32 x, Channel_Set ch);
  void multiply_by_image(Image *other, Channel_Set ch);
  void screen_by_image(Image *other, Channel_Set ch);
  void add(i16 x, Channel_Set ch);
  void subtract_by_image(Image *other, Channel_Set ch);
  void overlay_by_image(Image *other, Channel_Set ch);
  void rotate();
  void resize(i32 width, i32 height);
  i32 compare_to_image(Image *other) const;

  void read_from_path(String path, Arena *arena);
  void read_from_image(Image *other, Arena *arena);
  void write_to_path(String path) const;
  void write_to_image(Image *other, Arena *arena) const;

  void print_stats() const;
};
