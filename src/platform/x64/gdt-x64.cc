#include "gdt-x64.hpp"

namespace OS {

namespace x64 {

static GDT globalGDT;
static bool initialized = false;

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

TSS * GDT::DescTSS::GetPointer() {
  uint64_t ptrVal = 0;
  ptrVal |= base_0;
  ptrVal |= base_16 << 16;
  ptrVal |= base_24 << 24;
  ptrVal |= (uint64_t)base_32 << 0x20;
  return (TSS *)ptrVal;
}

GDT::GDTPointer GDT::GetCurrentPtr() {
  GDTPointer result;
  __asm__("sgdt (%0)" : : "r" (&result));
  return result;
}

GDT & GDT::GetGlobal() {
  if (!initialized) {
    new(&globalGDT) GDT(GetCurrentPtr());
    initialized = true;
  }
  return globalGDT;
}

GDT::~GDT() {
  delete buffer;
}

GDT::GDT(GDT::GDTPointer currentPtr)
  : offset(currentPtr.limit + 1), initOffset(offset) {
  buffer = new uint8_t[0x10000];
  memcpy(buffer, (void *)currentPtr.start, offset);
  bzero(buffer + offset, 0x10000 - offset);
}

uint16_t GDT::AddTSS(TSS * aTSS) {
  assert(offset <= 0xfff0);
  
  DescTSS desc(aTSS);
  memcpy(buffer + offset, &desc, 0x10);
  
  uint16_t result = (uint16_t)offset;
  offset += 0x10;
  return result;
}

TSS * GDT::GetTSS(uint16_t sel) {
  return (TSS *)(buffer + sel);
}

size_t GDT::GetInitialOffset() {
  return initOffset;
}

int GDT::GetTSSIndex() {
  uint64_t reg;
  __asm__("xor %%rax, %%rax\n"
          "str %%ax" : "=a" (reg));
  return (int)((reg - initOffset) / 0x10);
}

void GDT::Set() {
  GDTPointer ptr = {0xffff, (uint64_t)buffer};
  // I don't think this needs to be aligned, and it won't be if we use GCC :(
  // assert(!((uint64_t)&ptr & 7));
  __asm__("lgdt (%0)" : : "r" (&ptr));
}

}

}
