#include <arch/x64/segments/gdt.hpp>
#include <cassert>
#include <cstring>
#include <new>

namespace OS {

namespace x64 {

static GDT globalGDT;

GDTPointer GDTPointer::GetCurrent() {
  GDTPointer result;
  __asm__("sgdt (%0)" : : "r" (&result));
  return result;
}

void GDT::Initialize() {
  new(&globalGDT) GDT();
}

GDT & GDT::GetGlobal() {
  return globalGDT;
}

GDT::GDT() {
  GDTPointer curPtr = GDTPointer::GetCurrent();
  
  offset = initOffset = (size_t)curPtr.limit + 1;
  
  buffer = new uint8_t[0x10000];
  memcpy(buffer, (void *)curPtr.start, offset);
  bzero(buffer + offset, 0x10000 - offset);
}

void GDT::Set() {
  GDTPointer ptr = {0xffff, (uint64_t)buffer};
  __asm__("lgdt (%0)" : : "r" (&ptr));
}

uint16_t GDT::AddTSS(TSS * aTSS) {
  assert(offset <= 0xfff0);
  
  uint16_t result = (uint16_t)offset;
  
  TSSDescriptor desc(aTSS);
  memcpy(buffer + offset, &desc, 0x10);
  offset += 0x10;
  
  return result;
}

TSS * GDT::GetTSS(uint16_t sel) {
  TSSDescriptor * desc = (TSSDescriptor *)(buffer + sel);
  return desc->GetPointer();
}

uint16_t GDT::GetFirstTSS() {
  return (uint16_t)initOffset;
}

int GDT::GetTSSIndex() {
  uint16_t reg;
  __asm__("str %%ax" : "=a" (reg));
  return GetTSSIndex(reg);
}

int GDT::GetTSSIndex(uint16_t sel) {
  return (int)(sel - initOffset) / 0x10;
}

}

}
