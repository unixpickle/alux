#ifndef __ALUX_IPC_DATA_HPP__
#define __ALUX_IPC_DATA_HPP__

#include <anarch/types>
#include <anarch/stdint>
#include <anarch/stddef>

namespace Alux {

union Datum {
  size_t vSize;
  PhysSize pSize;
  
  VirtAddr vAddr;
  PhysAddr pAddr;
  
  uint64_t integer64;
};

struct Data {
  Datum data[4];
};

}

#endif
