#include <cstring>

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "base/base.hpp"
#include "image.hpp"

static u8 clamp(i32 val, i32 min, i32 max);
static f32 normalize(u8 val);
static u8 revert(f32 val);

// Image ///////////////////////////////////////////////////////////////////////

void Image::multiply_by_image(Image *other, ChannelSet ch)
{
  for (u32 i = 0; i < this->size(); i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];

    if (has_bit(ch, Channel_R))
    {
      r = revert(normalize(r) * normalize(other->data[i+0]));
    }
    if (has_bit(ch, Channel_G))
    {
      g = revert(normalize(g) * normalize(other->data[i+1]));
    }
    if (has_bit(ch, Channel_B))
    {
      b = revert(normalize(b) * normalize(other->data[i+2]));
    }
  }
}

void Image::scale_color(f32 x, ChannelSet ch)
{
  for (u32 i = 0; i < this->size(); i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];

    if (has_bit(ch, Channel_R))
    {
      r = clamp(revert(normalize(r)) * x, 0, 255);
    }
    if (has_bit(ch, Channel_G))
    {
      g = clamp(revert(normalize(g)) * x, 0, 255);
    }
    if (has_bit(ch, Channel_B))
    {
      b = clamp(revert(normalize(b)) * x, 0, 255);
    }
  }
}

void Image::screen_by_image(Image *other, ChannelSet ch)
{
  for (u32 i = 0; i < this->size(); i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];

    if (has_bit(ch, Channel_R))
    {
      r = revert(1 - (1 - normalize(r)) * (1 - normalize(other->data[i+0])));
    }
    if (has_bit(ch, Channel_G))
    {
      g = revert(1 - (1 - normalize(g)) * (1 - normalize(other->data[i+1])));
    }
    if (has_bit(ch, Channel_B))
    {
      b = revert(1 - (1 - normalize(b)) * (1 - normalize(other->data[i+2])));
    }
  }
}

void Image::add(i16 x, ChannelSet ch)
{
  for (u32 i = 0; i < this->size(); i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];

    if (has_bit(ch, Channel_R))
    {
      r = clamp(r + x, 0, 255);
    }
    if (has_bit(ch, Channel_G))
    {
      g = clamp(g + x, 0, 255);
    }
    if (has_bit(ch, Channel_B))
    {
      b = clamp(b + x, 0, 255);
    }
  }
}

void Image::subtract_by_image(Image *other, ChannelSet ch)
{
  for (u32 i = 0; i < this->size(); i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];

    if (has_bit(ch, Channel_R))
    {
      r = clamp(r - other->data[i+0], 0, 255);
    }
    if (has_bit(ch, Channel_G))
    {
      g = clamp(g + other->data[i+1], 0, 255);
    }
    if (has_bit(ch, Channel_B))
    {
      b = clamp(b + other->data[i+2], 0, 255);
    }
  }
}

void Image::overlay_by_image(Image *other, ChannelSet ch)
{
  for (u32 i = 0; i < this->size(); i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];

    if (has_bit(ch, Channel_R))
    {
      if (normalize(other->data[i+0]) <= 0.5f)
      {
        r = revert(2 * normalize(r) * normalize(other->data[i+0]));
      }
      else
      {
        r = revert(1 - (2 * ((1 - normalize(r)) * (1 - normalize(other->data[i+0])))));
      }
    }
    if (has_bit(ch, Channel_G))
    {
      if (normalize(other->data[i+1]) <= 0.5f)
      {
        g = revert(2 * normalize(g) * normalize(other->data[i+1]));
      }
      else
      {
        g = revert(1 - (2 * ((1 - normalize(g)) * (1 - normalize(other->data[i+1])))));
      }
    }
    if (has_bit(ch, Channel_B))
    {
      if (normalize(other->data[i+2]) <= 0.5f)
      {
        b = revert(2 * normalize(b) * normalize(other->data[i+2]));
      }
      else
      {
        b = revert(1 - (2 * ((1 - normalize(b)) * (1 - normalize(other->data[i+2])))));
      }
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

void Image::resize(i32 width, i32 height)
{

}

i32 Image::compare_to_image(Image *other) const
{
  i32 diff = 0;

  for (u32 i = 0; i < this->size(); i += 3)
  {
    byte &r = this->data[i+0];
    byte &g = this->data[i+1];
    byte &b = this->data[i+2];
    
    if (r != other->data[i+0] || g != other->data[i+1] || b != other->data[i+2])
    {
      diff++;
    }
  }

  return diff;
}

void Image::read_from_path(String path, Arena *arena)
{
  byte *temp = stbi_load(path.raw_data(), 
                         &this->width, 
                         &this->height, 
                         &this->channels,
                         4);

  this->data = arena_push(arena, byte, this->size());
  memcpy(this->data, temp, this->size());
  free(temp);
}

void Image::read_from_image(Image *other, Arena *arena)
{
  this->width = other->width;
  this->height = other->height;
  this->channels = other->channels;
  this->data = arena_push(arena, byte, other->size());
  memcpy(this->data, other->data, other->size());
}

void Image::write_to_path(String path) const
{
  stbi_write_tga(path.raw_data(), 
                 this->width, 
                 this->height, 
                 this->channels, 
                 this->data);
}

void Image::write_to_image(Image *other, Arena *arena) const
{
  other->width = this->width;
  other->height = this->height;
  other->channels = this->channels;
  other->data = arena_push(arena, byte, this->size());
  memcpy(other->data, this->data, this->size());
}

void Image::print_stats() const
{
  printf("   WIDTH: %i\n", this->width);
  printf("  HEIGHT: %i\n", this->height);
  printf("CHANNELS: %i\n", this->channels);
}

// Math ////////////////////////////////////////////////////////////////////////

static 
u8 clamp(i32 val, i32 min, i32 max)
{
  if (val < min) return (u8) min;
  if (val > max) return (u8) max;

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
