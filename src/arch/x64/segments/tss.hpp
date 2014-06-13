#ifndef __X64_TSS_HPP__
#define __X64_TSS_HPP__

#include <macros>
#include <cstdint>

namespace OS {

namespace x64 {

class TSS {
public:
  uint32_t res0;
  uint64_t rsp[3];
  uint64_t res1;
  uint64_t ist[7];
  uint64_t res2;
  uint16_t res3;
  uint16_t ioplBase;
} OS_PACKED;

class TSSDescriptor {
public:
  uint16_t limit_0;
  uint16_t base_0;
  uint8_t base_16;
  unsigned type : 4;
  unsigned res0 : 1;
  unsigned dpl : 2;
  unsigned present : 1;
  unsigned limit_16 : 4;
  unsigned available : 1;
  unsigned res1 : 2;
  unsigned granularity : 1;
  uint8_t base_24;
  uint32_t base_32;
  uint32_t res2;

  TSSDescriptor(TSS * tss);
  TSS * GetPointer();
} OS_PACKED;

}

}

#endif
