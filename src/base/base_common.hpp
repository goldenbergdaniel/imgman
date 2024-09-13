#pragma once

#include <assert.h>
#include <stdint.h>

typedef uint8_t byte;
typedef uint8_t b8;
typedef uint16_t b16;
typedef uint32_t b32;
typedef uint64_t b64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

#define size_of(T) sizeof(T)
#define align_of(T) _Alignof(T)

#define has_bit(set, bit) ((set & bit) != 0)

#define log_color(caption, r, g, b) (printf("%s%i, %i, %i\n", caption, (i32) r, (i32) g, (i32) b))
