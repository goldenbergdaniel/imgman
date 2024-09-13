#include "base_arena.hpp"

#ifdef USE_CUSTOM_ALLOC
#ifdef PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdio.h>

#ifdef PLATFORM_UNIX
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/param.h>
#endif

#ifdef PLATFORM_MACOS
#include <mach-o/dyld.h>
#undef bool
#endif
#endif

// @Arena //////////////////////////////////////////////////////////////////////

#define PAGES_PER_COMMIT 4

#ifndef SCRATCH_SIZE
#define SCRATCH_SIZE MiB(64)
#endif

static byte *align_ptr(byte *ptr, u32 align);

thread_local Arena _scratch_1;
thread_local Arena _scratch_2;

Arena::Arena(u64 size)
{
  #ifdef USE_CUSTOM_ALLOC
  arena.memory = os_reserve_vm(nullptr, size);
  #else
  this->memory = new byte[size];
  #endif
  this->allocated = this->memory;
  this->committed = this->memory;
  this->size = size;
  this->decommit_on_clear = true;
}

Arena::~Arena()
{
  this->release();
}

void Arena::release()
{
  #ifdef USE_CUSTOM_ALLOC
  os_release_vm(arena->memory, 0);
  #else
  delete[] this->memory;
  #endif
  this->memory = nullptr;
  this->allocated = nullptr;
  this->size = 0;
}

byte *Arena::push(u64 size, u64 align)
{
  byte *ptr = align_ptr(this->allocated, align);
  this->allocated = ptr + size;

  #ifdef USE_CUSTOM_ALLOC
  if (arena->committed < arena->allocated)
  {
    u64 granularity = os_get_page_size() * PAGES_PER_COMMIT;
    u64 size_to_commit = (u64) (arena->allocated - arena->committed);
    size_to_commit += -size_to_commit & (granularity - 1);

    bool ok = os_commit_vm(arena->committed, size_to_commit);
    if (!ok)
    {
      #ifdef PLATFORM_WINDOWS
      i32 code = GetLastError();
      printf("Error %i. Failed to commit! Size: %llu\n", code, size_to_commit);
      OutputDebugStringA("Failed to commit!\n");
      #endif
      assert(0);
    }

    arena->committed += size_to_commit;
  }
  #endif
  
  return ptr;
}

void Arena::pop(u64 size)
{
  this->allocated -= size;
  
  // Zero the memory
  u64 start_idx = (u64) (this->allocated - this->memory) - 1;
  u64 end_idx = start_idx + size;
  for (u64 i = start_idx; i < end_idx; i++)
  {
    this->allocated[i] = 0;
  }
}

void Arena::clear()
{
  #ifdef USE_CUSTOM_ALLOC
  if (arena->decommit_on_clear)
  {
    u64 commit_size = arena->committed - arena->memory;
    u64 page_size = os_get_page_size();

    // If committed pages > 16, decommit pages after 16th
    // TODO(dg): Verify that this is correct
    u64 page_limit = page_size * 16;
    if (commit_size > page_limit)
    {
      byte *start_addr = arena->memory + page_limit;
      os_decommit_vm(start_addr, commit_size - page_limit);
      arena->committed = start_addr;
    }
  }
  #endif

  for (u64 i = 0; i < this->allocated - this->memory; i++)
  {
    this->memory[i] = 0;
  }

  this->allocated = this->memory;
}

void init_scratch_arenas(void)
{
  _scratch_1 = Arena(SCRATCH_SIZE);
  _scratch_1.id = 0;
  _scratch_2 = Arena(SCRATCH_SIZE);
  _scratch_2.id = 1;
}

Arena get_scratch_arena(Arena *conflict)
{
  Arena result = _scratch_1;
  
  if (conflict != nullptr)
  {
    if (conflict->id == _scratch_1.id)
    {
      result = _scratch_2;
    }
    else if (conflict->id == _scratch_2.id)
    {
      result = _scratch_1;
    }
  }

  return result;
}

static
byte *align_ptr(byte *ptr, u32 align)
{
	u64 result = (u64) ptr;
  u64 remainder = result % align;
  if (remainder != 0)
  {
    result += align - remainder;
  }

	return (byte *) result;
}
