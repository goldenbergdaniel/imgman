#include "base/base.hpp"
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
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
      case CHANNEL_R:
        this->pixels[i].r = revert(normalize(this->pixels[i].r) * normalize(other->pixels[i].r));
        break;
      case CHANNEL_G:
        this->pixels[i].g = revert(normalize(this->pixels[i].g) * normalize(other->pixels[i].g));
        break;
      case CHANNEL_B:
        this->pixels[i].b = revert(normalize(this->pixels[i].b) * normalize(other->pixels[i].b));
        break;
      case CHANNEL_RGB:
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
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
    case CHANNEL_R:
      this->pixels[i].r = clamp(revert(normalize(this->pixels[i].r)) * x, 0, 255);
      break;
    case CHANNEL_G:
      this->pixels[i].g = clamp(revert(normalize(this->pixels[i].g)) * x, 0, 255);
      break;
    case CHANNEL_B:
      this->pixels[i].b = clamp(revert(normalize(this->pixels[i].b)) * x, 0, 255);
      break;
    case CHANNEL_RGB:
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
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
    case CHANNEL_R:
      this->pixels[i].r = revert(1 - (1 - normalize(this->pixels[i].r)) * (1 - normalize(other->pixels[i].r)));
      break;
    case CHANNEL_G:
      this->pixels[i].g = revert(1 - (1 - normalize(this->pixels[i].g)) * (1 - normalize(other->pixels[i].g)));
      break;
    case CHANNEL_B:
      this->pixels[i].b = revert(1 - (1 - normalize(this->pixels[i].b)) * (1 - normalize(other->pixels[i].b)));
      break;
    case CHANNEL_RGB:
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
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
    case CHANNEL_R:
      this->pixels[i].r = clamp(this->pixels[i].r + x, 0, 255);
      break;
    case CHANNEL_G:
      this->pixels[i].g = clamp(this->pixels[i].g + x, 0, 255);
      break;
    case CHANNEL_B:
      this->pixels[i].b = clamp(this->pixels[i].b + x, 0, 255);
      break;
    case CHANNEL_RGB:
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
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
    case CHANNEL_R:
      this->pixels[i].r = clamp(this->pixels[i].r - this->pixels[i].r, 0, 255);
      break;
    case CHANNEL_G:
      this->pixels[i].g = clamp(this->pixels[i].g - this->pixels[i].g, 0, 255);
      break;
    case CHANNEL_B:
      this->pixels[i].b = clamp(this->pixels[i].b - this->pixels[i].b, 0, 255);
      break;
    case CHANNEL_RGB:
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
  u32 size = this->width * this->height;

  for (u32 i = 0; i < size; i++)
  {
    switch (ch)
    {
    case CHANNEL_R:
      if (normalize(other->pixels[i].r) <= 0.5f)
      {
        this->pixels[i].r = revert(2 * normalize(this->pixels[i].r) * normalize(other->pixels[i].r));
      }
      else
      {
        this->pixels[i].r = revert(1 - (2 * ((1 - normalize(this->pixels[i].r)) * (1 - normalize(other->pixels[i].r)))));
      }
      break;
    case CHANNEL_G:
      if (normalize(other->pixels[i].g) <= 0.5f)
      {
        this->pixels[i].g = revert(2 * normalize(this->pixels[i].g) * normalize(other->pixels[i].g));
      }
      else
      {
        this->pixels[i].g = revert(1 - (2 * ((1 - normalize(this->pixels[i].g)) * (1 - normalize(other->pixels[i].g)))));
      }
      break;
    case CHANNEL_B:
      if (normalize(other->pixels[i].b) <= 0.5f)
      {
        this->pixels[i].b = revert(2 * normalize(this->pixels[i].b) * normalize(other->pixels[i].b));
      }
      else
      {
        this->pixels[i].b = revert(1 - (2 * ((1 - normalize(this->pixels[i].b)) * (1 - normalize(other->pixels[i].b)))));
      }
      break;
    case CHANNEL_RGB:
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
  u32 size = this->width * this->height;

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
  u32 size = this->width * this->height;

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

void Image::read(String path)
{

}

void Image::write(String path)
{

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
