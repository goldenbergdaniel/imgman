#include "base.hpp"

#ifdef PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

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

#include <stdio.h>

byte *os_reserve_vm(byte *addr, u64 size)
{
  byte *result = NULL;
  
  #ifdef PLATFORM_WINDOWS
  result = VirtualAlloc((void *) addr, size, MEM_RESERVE, PAGE_NOACCESS);
  #endif

  #ifdef PLATFORM_UNIX
  result = (byte *) mmap(NULL, size, PROT_NONE, MAP_ANON | MAP_PRIVATE, -1, 0);
  if (result == MAP_FAILED)
  {
    printf("mmap failed with size %lu \n", size);
    assert(false);
  }
  #endif

  return result;
}

bool os_commit_vm(byte *addr, u64 size)
{
  bool result = true;

  #ifdef PLATFORM_WINDOWS
  byte *ptr = VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE);
  if (ptr == NULL)
  {
    result = GetLastError();
  }
  #endif

  #ifdef PLATFORM_UNIX
  i32 err = mprotect(addr, size, PROT_READ | PROT_WRITE);
  if (err != 0)
  {
    printf("Failed to commit with mprotect.\n");
    result = false;
  }

  #endif

  return result;
}

bool os_decommit_vm(byte *addr, u64 size)
{
  bool result = true;

  #ifdef PLATFORM_WINDOWS
  result = VirtualFree(addr, size, MEM_DECOMMIT);
  #endif

  #ifdef PLATFORM_UNIX
  i32 err = mprotect(addr, size, PROT_NONE);
  if (err != 0)
  {
    printf("Failed to decommit with mprotect.\n");
    result = false;
  }
  #endif

  return result;
}

void os_release_vm(byte *ptr, u64 size)
{
  #ifdef PLATFORM_WINDOWS
  VirtualFree(ptr, size, MEM_RELEASE);
  #endif

  #ifdef PLATFORM_UNIX
  munmap(ptr, size);
  #endif
}

// TODO(dg): This should be cached somewhere
u64 os_get_page_size(void)
{
  u64 result = 0;

  #ifdef PLATFORM_WINDOWS
  SYSTEM_INFO info = {0};
  GetSystemInfo(&info);
  result = info.dwPageSize;
  #endif

  #ifdef PLATFORM_UNIX
  // result = getpagesize();
  result = 4096;
  #endif

  return result;
}
