#include <arch/x64/common.hpp>
#include <critical>

namespace OS {

namespace x64 {

void OutB(uint16_t port, uint8_t byte) {
  __asm__("outb %%al, %%dx" : : "a" (byte), "d" (port));
}

void CPUID(uint32_t eax, uint32_t * ebx, uint32_t * edx, uint32_t * ecx) {
  AssertCritical();
  uint32_t ebxIn = ebx ? *ebx : 0;
  uint32_t ecxIn = ecx ? *ecx : 0;
  uint32_t edxIn = edx ? *edx : 0;
  uint64_t rbxOut, rcxOut, rdxOut;
  __asm__("cpuid"
          : "=b" (rbxOut), "=c" (rcxOut), "=d" (rdxOut)
          : "b" (ebxIn), "c" (ecxIn), "d" (edxIn), "a" (eax));
  if (ebx) *ebx = (uint32_t)rbxOut;
  if (ecx) *ecx = (uint32_t)rcxOut;
  if (edx) *edx = (uint32_t)rdxOut;
}

uint64_t ReadMSR(uint32_t cell) {
  AssertCritical();
  uint64_t higher;
  uint64_t lower;
  __asm__("rdmsr"
          : "=d" (higher), "=a" (lower)
          : "c" (cell));
  return (higher << 0x20) | lower;
}

void WriteMSR(uint32_t cell, uint64_t value) {
  AssertCritical();
  uint64_t lower = value & 0xffffffff;
  uint64_t higher = value >> 0x20;
  __asm__("wrmsr" : : "c" (cell), "d" (higher), "a" (lower));
}

}

}
