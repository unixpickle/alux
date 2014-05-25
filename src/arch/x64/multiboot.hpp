#ifndef __X64_MULTIBOOT_HPP__
#define __X64_MULTIBOOT_HPP__

#include <common>

namespace OS {

namespace x64 {

class MultibootBootInfo {
public:
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
} OS_PACKED;

class MultibootMmapInfo {
public:
  uint32_t size;
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
} OS_PACKED;

}

}

#endif
