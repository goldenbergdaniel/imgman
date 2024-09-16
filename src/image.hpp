#pragma once

#include "base/base.hpp"

enum Channel 
{
  Channel_R = 1 << 0, 
  Channel_G = 2 << 1, 
  Channel_B = 3 << 2, 
};

enum FormatType
{
  FormatType_Nil,

  FormatType_PNG,
  FormatType_JPG,
  FormatType_TGA,
  FormatType_BMP,
};

typedef b64 ChannelSet;

class Image
{
private:
  byte *data = nullptr;
  FormatType type = FormatType_Nil;

public:
  i32 width = 0;
  i32 height = 0;
  i32 channels = 0;

  u64 size() const { return this->width * this->height; }
  byte *raw_data() const { return this->data; }
  FormatType format_type() const { return this->format_type(); }

  void scale_color(f32 x, ChannelSet ch);
  void multiply_by_image(Image *other, ChannelSet ch);
  void screen_by_image(Image *other, ChannelSet ch);
  void add(i16 x, ChannelSet ch);
  void subtract_by_image(Image *other, ChannelSet ch);
  void overlay_by_image(Image *other, ChannelSet ch);
  void rotate();
  void resize(i32 width, i32 height);
  i32 compare_to_image(Image *other) const;

  void read_from_path(String path, Arena *arena);
  void read_from_image(Image *other, Arena *arena);
  void write_to_path(String path) const;
  void write_to_image(Image *other, Arena *arena) const;

  void print_stats() const;
};
