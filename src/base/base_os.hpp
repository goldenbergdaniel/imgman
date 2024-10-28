#pragma once

#include "base_common.hpp"

byte *os_reserve_vm(byte *addr, u64 size);
bool os_commit_vm(byte *addr, u64 size);
bool os_decommit_vm(byte *addr, u64 size);
void os_release_vm(byte *addr, u64 size);

u64 os_get_page_size(void);
