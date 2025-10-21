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
typedef intptr_t iptr;
typedef uintptr_t uptr;
typedef const char *cstr;

#define size_of(T) sizeof(T)
#define align_of(T) alignof(T)

#define has_bit(set, bit) ((set & bit) != 0)

#if defined(_WIN32)
  #define PLATFORM_WINDOWS
#elif defined(__APPLE__)
  #define PLATFORM_MACOS
  #define PLATFORM_UNIX
#elif defined (__linux__)
  #define PLATFORM_LINUX
  #define PLATFORM_UNIX
#endif

#define USE_CUSTOM_ALLOC
