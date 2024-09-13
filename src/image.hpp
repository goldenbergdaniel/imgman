#pragma once

#include <fstream>
#include "globals.hpp"

#define IMG_MAX_SIZE 512*512

enum Channel 
{
  R, G, B, RGB
};

struct Header 
{
  i8 id_len;
  i8 color_map_type;
  i8 data_type_code;
  i16 color_map_origin;
  i16 color_map_len;
  i8 color_map_depth;
  i16 x_origin;
  i16 y_origin;
  i16 width;
  i16 height;
  i8 bpp;
  i8 image_desc;
};

struct Color
{
  u8 r, g, b;

  Color();
  Color(u8 r, u8 g, u8 b);
};

class Image
{
private:
  Header header;
  Color *pixels;

public:
  Image();
  ~Image();

  void scale(f32 x, Channel ch);
  void multiply(Image *other, Channel ch);
  void screen(Image *other, Channel ch);
  void add(i16 x, Channel ch);
  void subtract(Image *other, Channel ch);
  void overlay(Image *other, Channel ch);
  void rotate();
  i32 compare(Image *other);

  void read(std::ifstream *stream);
  void write(std::ofstream *stream, Channel ch);

private:
  void write_pixels(std::ofstream *stream, Channel ch);
  void write_header(std::ofstream *stream);
};
