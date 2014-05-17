
/*

#include "gdt-x64.hpp"

namespace OS {

namespace x64 {

GDT::DescTSS::DescTSS(TSS * tss)
  : limit_0(0x67), type(9), res0(0), dpl(0), present(1), limit_16(0),
    available(1), res1(0), granularity(0), res2(0) {
  // as much as I love initializer lists, I like code too.
  uint64_t ptrVal = (uint64_t)tss;
  base_0 = ptrVal & 0xffff;
  base_16 = (ptrVal >> 16) & 0xff;
  base_24 = (ptrVal >> 24) & 0xff;
  base_32 = ptrVal >> 0x20;
}

GDTPointer & GDT::GetCurrentPtr() {
  // TODO: nyi
}

GDT & GDT::GetGlobal() {
  // TODO: nyi
}

GDT::GDT(const GDTPointer & currentPtr) {
  // TODO: nyi
}

uint16_t GDT::AddTSS(TSS * aTSS) {
  // TODO: nyi
}

};

}

}

*/
