#ifndef __PLATFORM_X64_MULTIBOOT_X64_H__
#define __PLATFORM_X64_MULTIBOOT_X64_H__

#include <cstdint>
#include <startup/entry.hpp>
#include "common-x64.hpp"
#include "printing-x64.hpp"

namespace OS {

namespace x64 {

typedef struct {
  uint32_t flags;
  uint32_t mem_lower;
  uint32_t mem_upper;
  uint32_t boot_device;
  uint32_t cmdline;
  uint32_t mods_count;
  uint32_t mods_addr;
  char syms[0x10];
  uint32_t mmap_length;
  uint32_t mmap_addr;
  uint32_t drives_length;
  uint32_t drives_addr;
  uint32_t config_table;
  uint32_t boot_loader_name;
  uint32_t apm_table;
} OS_PACKED MultibootBootInfo;

typedef struct {
  uint32_t size;
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
} OS_PACKED MultibootMmapInfo;

}

}

extern "C" {

void MbootEntry(void * mbootPtr);

/**
 * Only used when not compiling on OS X.
 */
void _MbootEntry(void * mbootPtr);

}

#endif

