#include <fstream>

#include "globals.hpp"
#include "image.hpp"

static u8 clamp(i32 val, i32 min, i32 max);
static f32 normalize(u8 val);
static u8 revert(f32 val);

// Color ///////////////////////////////////////////////////////////////////////

Color::Color()
{
  this->r = 0;
  this->g = 0;
  this->g = 0;
}

Color::Color(u8 r, u8 g, u8 b)
{
  this->r = r;
  this->g = g;
  this->g = b;
}

// Image ///////////////////////////////////////////////////////////////////////

Image::Image()
{
  this->pixels = new Color[IMG_MAX_SIZE];
}

Image::~Image()
{
  delete[] this->pixels;
}

void Image::multiply(Image *other, Channel ch)
{
  u32 size = this->header.width * this->header.height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
      case Channel::R:
        this->pixels[i].r = revert(normalize(this->pixels[i].r) * normalize(other->pixels[i].r));
        break;
      case Channel::G:
        this->pixels[i].g = revert(normalize(this->pixels[i].g) * normalize(other->pixels[i].g));
        break;
      case Channel::B:
        this->pixels[i].b = revert(normalize(this->pixels[i].b) * normalize(other->pixels[i].b));
        break;
      case Channel::RGB:
        this->pixels[i].r = revert(normalize(this->pixels[i].r) * normalize(other->pixels[i].r));
        this->pixels[i].g = revert(normalize(this->pixels[i].g) * normalize(other->pixels[i].g));
        this->pixels[i].b = revert(normalize(this->pixels[i].b) * normalize(other->pixels[i].b));
        break;
      default: break;
    }
  }
}

void Image::scale(f32 x, Channel ch)
{
  u32 size = this->header.width * this->header.height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
    case Channel::R:
      this->pixels[i].r = clamp(revert(normalize(this->pixels[i].r)) * x, 0, 255);
      break;
    case Channel::G:
      this->pixels[i].g = clamp(revert(normalize(this->pixels[i].g)) * x, 0, 255);
      break;
    case Channel::B:
      this->pixels[i].b = clamp(revert(normalize(this->pixels[i].b)) * x, 0, 255);
      break;
    case Channel::RGB:
      this->pixels[i].r = clamp(revert(normalize(this->pixels[i].r) * x), 0, 255);
      this->pixels[i].g = clamp(revert(normalize(this->pixels[i].g) * x), 0, 255);
      this->pixels[i].b = clamp(revert(normalize(this->pixels[i].b) * x), 0, 255);
      break;
    default: break;
    }
  }
}

void Image::screen(Image *other, Channel ch)
{
  u32 size = this->header.width * this->header.height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
    case Channel::R:
      this->pixels[i].r = revert(1 - (1 - normalize(this->pixels[i].r)) * (1 - normalize(other->pixels[i].r)));
      break;
    case Channel::G:
      this->pixels[i].g = revert(1 - (1 - normalize(this->pixels[i].g)) * (1 - normalize(other->pixels[i].g)));
      break;
    case Channel::B:
      this->pixels[i].b = revert(1 - (1 - normalize(this->pixels[i].b)) * (1 - normalize(other->pixels[i].b)));
      break;
    case Channel::RGB:
      this->pixels[i].r = revert(1 - (1 - normalize(this->pixels[i].r)) * (1 - normalize(other->pixels[i].r)));
      this->pixels[i].g = revert(1 - (1 - normalize(this->pixels[i].g)) * (1 - normalize(other->pixels[i].g)));
      this->pixels[i].b = revert(1 - (1 - normalize(this->pixels[i].b)) * (1 - normalize(other->pixels[i].b)));
      break;
    default: break;
    }
  }
}

void Image::add(i16 x, Channel ch)
{
  u32 size = this->header.width * this->header.height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
    case Channel::R:
      this->pixels[i].r = clamp(this->pixels[i].r + x, 0, 255);
      break;
    case Channel::G:
      this->pixels[i].g = clamp(this->pixels[i].g + x, 0, 255);
      break;
    case Channel::B:
      this->pixels[i].b = clamp(this->pixels[i].b + x, 0, 255);
      break;
    case Channel::RGB:
      this->pixels[i].r = clamp(this->pixels[i].r + x, 0, 255);
      this->pixels[i].g = clamp(this->pixels[i].g + x, 0, 255);
      this->pixels[i].b = clamp(this->pixels[i].b + x, 0, 255);
      break;
    default:
      break;
    }
  }
}

void Image::subtract(Image *img, Channel ch)
{
  u32 size = this->header.width * this->header.height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
    case Channel::R:
      this->pixels[i].r = clamp(this->pixels[i].r - this->pixels[i].r, 0, 255);
      break;
    case Channel::G:
      this->pixels[i].g = clamp(this->pixels[i].g - this->pixels[i].g, 0, 255);
      break;
    case Channel::B:
      this->pixels[i].b = clamp(this->pixels[i].b - this->pixels[i].b, 0, 255);
      break;
    case Channel::RGB:
      this->pixels[i].r = clamp(this->pixels[i].r - this->pixels[i].r, 0, 255);
      this->pixels[i].g = clamp(this->pixels[i].g - this->pixels[i].g, 0, 255);
      this->pixels[i].b = clamp(this->pixels[i].b - this->pixels[i].b, 0, 255);
      break;
    default: break;
    }
  }
}

void Image::overlay(Image *other, Channel ch)
{
  u32 size = this->header.width * this->header.height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
    case Channel::R:
      if (normalize(other->pixels[i].r) <= 0.5f)
      {
        this->pixels[i].r = revert(2 * normalize(this->pixels[i].r) * normalize(other->pixels[i].r));
      }
      else
      {
        this->pixels[i].r = revert(1 - (2 * ((1 - normalize(this->pixels[i].r)) * (1 - normalize(other->pixels[i].r)))));
      }
      break;
    case Channel::G:
      if (normalize(other->pixels[i].g) <= 0.5f)
      {
        this->pixels[i].g = revert(2 * normalize(this->pixels[i].g) * normalize(other->pixels[i].g));
      }
      else
      {
        this->pixels[i].g = revert(1 - (2 * ((1 - normalize(this->pixels[i].g)) * (1 - normalize(other->pixels[i].g)))));
      }
      break;
    case Channel::B:
      if (normalize(other->pixels[i].b) <= 0.5f)
      {
        this->pixels[i].b = revert(2 * normalize(this->pixels[i].b) * normalize(other->pixels[i].b));
      }
      else
      {
        this->pixels[i].b = revert(1 - (2 * ((1 - normalize(this->pixels[i].b)) * (1 - normalize(other->pixels[i].b)))));
      }
      break;
    case Channel::RGB:
      if (normalize(other->pixels[i].r) <= 0.5f)
      {
        this->pixels[i].r = revert(2 * normalize(this->pixels[i].r) * normalize(other->pixels[i].r));
      }
      else
      {
        this->pixels[i].r = revert(1 - (2 * ((1 - normalize(this->pixels[i].r)) * (1 - normalize(other->pixels[i].r)))));
      }

      if (normalize(other->pixels[i].g) <= 0.5f)
      {
        this->pixels[i].g = revert(2 * normalize(this->pixels[i].g) * normalize(other->pixels[i].g));
      }
      else
      {
        this->pixels[i].g = revert(1 - (2 * ((1 - normalize(this->pixels[i].g)) * (1 - normalize(other->pixels[i].g)))));
      }
      
      if (normalize(other->pixels[i].b) <= 0.5f)
      {
        this->pixels[i].b = revert(2 * normalize(this->pixels[i].b) * normalize(other->pixels[i].b));
      }
      else
      {
        this->pixels[i].b = revert(1 - (2 * ((1 - normalize(this->pixels[i].b)) * (1 - normalize(other->pixels[i].b)))));
      }
      break;
    default: break;
    }
  }
}

void Image::rotate()
{
  u32 size = this->header.width * this->header.height;

  for (u32 i = 0; i < size/2; i++)
  {
    Color temp = this->pixels[i];
    this->pixels[i] = this->pixels[size-1-i];
    this->pixels[size-1-i] = temp;
  }
}

i32 Image::compare(Image *other)
{
  i32 diff = 0;
  u32 size = this->header.width * this->header.height;

  if (this->header.bpp != other->header.bpp)
  {
    printf("bpp: %i   %i\n", this->header.bpp, other->header.bpp);
  }

  if (this->header.color_map_depth != other->header.color_map_depth)
  {
    printf("color_map_depth: %i   %i\n", this->header.color_map_depth, other->header.color_map_depth);
  }

  if (this->header.color_map_len != other->header.color_map_len)
  {
    printf("color_map_len: %i   %i\n", this->header.color_map_len, other->header.color_map_len);
  }

  if (this->header.color_map_origin != other->header.color_map_origin)
  {
    printf("color_map_origin: %i   %i\n", this->header.color_map_origin, other->header.color_map_origin);
  }

  if (this->header.color_map_type != other->header.color_map_type)
  {
    printf("color_map_type: %i   %i\n", this->header.color_map_type, other->header.color_map_type);
  }

  if (this->header.data_type_code != other->header.data_type_code)
  {
    printf("data_type_code: %i   %i\n", this->header.data_type_code, other->header.data_type_code);
  }

  if (this->header.height != other->header.height)
  {
    printf("height: %i   %i\n", this->header.height, other->header.height);
  }

  if (this->header.width != other->header.width)
  {
    printf("width: %i   %i\n", this->header.width, other->header.width);
  }

  if (this->header.id_len != other->header.id_len)
  {
    printf("id_len: %i   %i\n", this->header.id_len, other->header.id_len);
  }

  if (this->header.image_desc != other->header.image_desc)
  {
    printf("image_desc: %i   %i\n", this->header.image_desc, other->header.image_desc);
  }

  if (this->header.x_origin != other->header.x_origin)
  {
    printf("x_origin: %i   %i\n", this->header.x_origin, other->header.x_origin);
  }

  if (this->header.y_origin != other->header.y_origin)
  {
    printf("y_origin: %i   %i\n", this->header.y_origin, other->header.y_origin);
  }

  for (u32 i = 0; i < size; i++)
  {
    if (this->pixels[i].r != other->pixels[i].r || 
        this->pixels[i].g != other->pixels[i].g ||
        this->pixels[i].b != other->pixels[i].b)
    {
      diff++;
    }
  }

  return diff;
}

void Image::read(std::ifstream *stream)
{
  stream->read((char *) &this->header.id_len, 1);
  stream->read((char *) &this->header.color_map_type, 1);
  stream->read((char *) &this->header.data_type_code, 1);
  stream->read((char *) &this->header.color_map_origin, 2);
  stream->read((char *) &this->header.color_map_len, 2);
  stream->read((char *) &this->header.color_map_depth, 1);
  stream->read((char *) &this->header.x_origin, 2);
  stream->read((char *) &this->header.y_origin, 2);
  stream->read((char *) &this->header.width, 2);
  stream->read((char *) &this->header.height, 2);
  stream->read((char *)&this->header.bpp, 1);
  stream->read((char *)&this->header.image_desc, 1);

  u32 size = this->header.width*this->header.height;

  for (u32 i = 0; i < size; i++)
  {
    stream->read((char *) &this->pixels[i].b, 1);
    stream->read((char *) &this->pixels[i].g, 1);
    stream->read((char *) &this->pixels[i].r, 1);
  }
}

void Image::write(std::ofstream *stream, Channel ch)
{
  write_header(stream);
  write_pixels(stream, ch);
}

void Image::write_pixels(std::ofstream *stream, Channel ch)
{
  u32 size = this->header.width*this->header.height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
      case Channel::R:
        stream->write((char *) &this->pixels[i].r, 1);
        stream->write((char *) &this->pixels[i].r, 1);
        stream->write((char *) &this->pixels[i].r, 1);
        break;
      case Channel::G:
        stream->write((char *) &this->pixels[i].g, 1);
        stream->write((char *) &this->pixels[i].g, 1);
        stream->write((char *) &this->pixels[i].g, 1);
        break;
      case Channel::B:
        stream->write((char *) &this->pixels[i].b, 1);
        stream->write((char *) &this->pixels[i].b, 1);
        stream->write((char *) &this->pixels[i].b, 1);
        break;
      case Channel::RGB:
        stream->write((char *) &this->pixels[i].b, 1);
        stream->write((char *) &this->pixels[i].g, 1);
        stream->write((char *) &this->pixels[i].r, 1);
        break;
      default:
        break;
    }
  }
}

void Image::write_header(std::ofstream *stream)
{
  stream->write((char *) &this->header.id_len, 1);
  stream->write((char *) &this->header.color_map_type, 1);
  stream->write((char *) &this->header.data_type_code, 1);
  stream->write((char *) &this->header.color_map_origin, 2);
  stream->write((char *) &this->header.color_map_len, 2);
  stream->write((char *) &this->header.color_map_depth, 1);
  stream->write((char *) &this->header.x_origin, 2);
  stream->write((char *) &this->header.y_origin, 2);
  stream->write((char *) &this->header.width, 2);
  stream->write((char *) &this->header.height, 2);
  stream->write((char *) &this->header.bpp, 1);
  stream->write((char *) &this->header.image_desc, 1);
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
