#pragma once

#include "base/base.hpp"

#define IMG_MAX_SIZE 512*512

enum Channel 
{
  CHANNEL_R, 
  CHANNEL_G, 
  CHANNEL_B, 
  CHANNEL_RGB,
};

class Image
{
private:
  i32 width;
  i32 height;
  byte *data;

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

  void read(String path);
  void write(String path);
};
