#ifndef __PLATFORM_X64_GDT_X64_H__
#define __PLATFORM_X64_GDT_X64_H__

#include <new>
#include "common-x64.hpp"

namespace OS {

namespace x64 {

class TSS {
  uint32_t res0;
  uint64_t rsp[3];
  uint64_t res1;
  uint64_t ist[7];
  uint64_t res2;
  uint16_t res3;
  uint16_t ioplBase;
} OS_PACKED;

class GDT {
public:
  class DescTSS {
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

    DescTSS(TSS * tss);
  } OS_PACKED;

  class GDTPointer {
    uint16_t limit;
    uint64_t start;
  } OS_PACKED;

private:
  void * buffer;
  size_t offset;

public:
  static GDTPointer & GetCurrentPtr(); // uses SGDT
  static GDT & GetGlobal();

  GDT() {
    Panic("This initializer is for the compiler only");
  }

  GDT(const GDTPointer & currentPtr);
  uint16_t AddTSS(TSS * aTSS);

};

}

}

#endif
