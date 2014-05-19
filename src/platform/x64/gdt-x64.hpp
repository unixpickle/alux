#ifndef __PLATFORM_X64_GDT_X64_HPP__
#define __PLATFORM_X64_GDT_X64_HPP__

#include <new>
#include <cstring>
#include "common-x64.hpp"

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

class GDT {
public:
  class DescTSS {
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

    DescTSS(TSS * tss);
    TSS * GetPointer();
  } OS_PACKED;
  static_assert(sizeof(DescTSS) == 0x10, "sizeof(DescTSS) == 0x10");

  class GDTPointer {
  public:
    uint16_t limit;
    uint64_t start;
  } OS_PACKED;
  static_assert(sizeof(GDTPointer) == 0xa, "sizeof(GDTPointer) == 0xa");

private:
  uint8_t * buffer;
  size_t offset;
  size_t initOffset;

public:
  static GDTPointer GetCurrentPtr(); // uses SGDT
  static GDT & GetGlobal();

  GDT() {
    Panic("This initializer is for the compiler only");
  }
  
  ~GDT();

  GDT(GDTPointer currentPtr);
  uint16_t AddTSS(TSS * aTSS);
  TSS * GetTSS(uint16_t sel);
  
  size_t GetInitialOffset();
  int GetTSSIndex();
  void Set();

};

}

}

#endif
