#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "base/base.hpp"
#include "image.hpp"

static u8 clamp(i32 val, i32 min, i32 max);
static f32 normalize(u8 val);
static u8 revert(f32 val);

// Image ///////////////////////////////////////////////////////////////////////

Image::Image()
{
  // this->pixels = new Color[IMG_MAX_SIZE];
}

Image::~Image()
{
  // delete[] this->pixels;
}

void Image::multiply(Image *other, Channel ch)
{
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];

    switch (ch)
    {
      case CHANNEL_R:
        r = revert(normalize(r) * normalize(other->data[i+0]));
        break;
      case CHANNEL_G:
        g = revert(normalize(g) * normalize(other->data[i+1]));
        break;
      case CHANNEL_B:
        b = revert(normalize(b) * normalize(other->data[i+2]));
        break;
      case CHANNEL_RGB:
        r = revert(normalize(r) * normalize(other->data[i+0]));
        g = revert(normalize(g) * normalize(other->data[i+1]));
        b = revert(normalize(b) * normalize(other->data[i+2]));
        break;
      default: break;
    }
  }
}

void Image::scale(f32 x, Channel ch)
{
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];

    switch (ch)
    {
      case CHANNEL_R:
        r = clamp(revert(normalize(r)) * x, 0, 255);
        break;
      case CHANNEL_G:
        g = clamp(revert(normalize(g)) * x, 0, 255);
        break;
      case CHANNEL_B:
        b = clamp(revert(normalize(b)) * x, 0, 255);
        break;
      case CHANNEL_RGB:
        r = clamp(revert(normalize(r) * x), 0, 255);
        g = clamp(revert(normalize(g) * x), 0, 255);
        b = clamp(revert(normalize(b) * x), 0, 255);
        break;
      default: break;
    }
  }
}

void Image::screen(Image *other, Channel ch)
{
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];

    switch (ch)
    {
      case CHANNEL_R:
        r = revert(1 - (1 - normalize(r)) * (1 - normalize(other->data[i+0])));
        break;
      case CHANNEL_G:
        g = revert(1 - (1 - normalize(g)) * (1 - normalize(other->data[i+1])));
        break;
      case CHANNEL_B:
        b = revert(1 - (1 - normalize(b)) * (1 - normalize(other->data[i+2])));
        break;
      case CHANNEL_RGB:
        r = revert(1 - (1 - normalize(r)) * (1 - normalize(other->data[i+0])));
        g = revert(1 - (1 - normalize(g)) * (1 - normalize(other->data[i+1])));
        b = revert(1 - (1 - normalize(b)) * (1 - normalize(other->data[i+2])));
        break;
      default: break;
    }
  }
}

void Image::add(i16 x, Channel ch)
{
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];

    switch (ch)
    {
      case CHANNEL_R:
        r = clamp(r + x, 0, 255);
        break;
      case CHANNEL_G:
        g = clamp(g + x, 0, 255);
        break;
      case CHANNEL_B:
        b = clamp(b + x, 0, 255);
        break;
      case CHANNEL_RGB:
        r = clamp(r + x, 0, 255);
        g = clamp(g + x, 0, 255);
        b = clamp(b + x, 0, 255);
        break;
      default:
        break;
    }
  }
}

void Image::subtract(Image *img, Channel ch)
{
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];

    switch (ch)
    {
      case CHANNEL_R:
        r = clamp(r - r, 0, 255);
        break;
      case CHANNEL_G:
        g = clamp(g - g, 0, 255);
        break;
      case CHANNEL_B:
        b = clamp(b - b, 0, 255);
        break;
      case CHANNEL_RGB:
        r = clamp(r - r, 0, 255);
        g = clamp(g - g, 0, 255);
        b = clamp(b - b, 0, 255);
        break;
      default: break;
    }
  }
}

void Image::overlay(Image *other, Channel ch)
{
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];

    switch (ch)
    {
      case CHANNEL_R:
        if (normalize(other->data[i+0]) <= 0.5f)
        {
          r = revert(2 * normalize(r) * normalize(other->data[i+0]));
        }
        else
        {
          r = revert(1 - (2 * ((1 - normalize(r)) * (1 - normalize(other->data[i+0])))));
        }
        break;
      case CHANNEL_G:
        if (normalize(other->data[i+1]) <= 0.5f)
        {
          g = revert(2 * normalize(g) * normalize(other->data[i+1]));
        }
        else
        {
          g = revert(1 - (2 * ((1 - normalize(g)) * (1 - normalize(other->data[i+1])))));
        }
        break;
      case CHANNEL_B:
        if (normalize(other->data[i+2]) <= 0.5f)
        {
          b = revert(2 * normalize(b) * normalize(other->data[i+2]));
        }
        else
        {
          b = revert(1 - (2 * ((1 - normalize(b)) * (1 - normalize(other->data[i+2])))));
        }
        break;
      case CHANNEL_RGB:
        if (normalize(other->data[i+0]) <= 0.5f)
        {
          r = revert(2 * normalize(r) * normalize(other->data[i+0]));
        }
        else
        {
          r = revert(1 - (2 * ((1 - normalize(r)) * (1 - normalize(other->data[i+0])))));
        }

        if (normalize(other->data[i+1]) <= 0.5f)
        {
          g = revert(2 * normalize(g) * normalize(other->data[i+1]));
        }
        else
        {
          g = revert(1 - (2 * ((1 - normalize(g)) * (1 - normalize(other->data[i+1])))));
        }
        
        if (normalize(other->data[i+2]) <= 0.5f)
        {
          b = revert(2 * normalize(b) * normalize(other->data[i+2]));
        }
        else
        {
          b = revert(1 - (2 * ((1 - normalize(b)) * (1 - normalize(other->data[i+2])))));
        }
        break;
      default: break;
    }
  }
}

void Image::rotate()
{
  // u32 size = this->width * this->height;

  // for (u32 i = 0; i < size/2; i++)
  // {
  //   Color temp = this->pixels[i];
  //   this->pixels[i] = this->pixels[size-1-i];
  //   this->pixels[size-1-i] = temp;
  // }
}

i32 Image::compare(Image *other)
{
  i32 diff = 0;
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i++)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];
    
    if (r != other->data[i+0] || 
        g != other->data[i+1] ||
        b != other->data[i+2])
    {
      diff++;
    }
  }

  return diff;
}

void Image::read(String path)
{
  this->data = stbi_load(path.raw_data(), 
                         &this->width, 
                         &this->height, 
                         nullptr,
                         3);
}

void Image::write(String path)
{
  stbi_write_tga(path.raw_data(), this->width, this->height, 3, this->data);
}

// Math ////////////////////////////////////////////////////////////////////////

static 
u8 clamp(i32 val, i32 min, i32 max)
{
  if (val < min) val = min;
  if (val > max) val = max;

  return (u8) val;
}

static 
f32 normalize(u8 val)
{
  return (f32) ((f32) val / 255.0f);
}

static
u8 revert(f32 val)
{
  return (u8) ((val * 255.0f) + 0.5f);
}
