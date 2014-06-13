#include <arch/x64/syscall/raw-handler.hpp>
#include <arch/x64/common.hpp>
#include <critical>

namespace OS {

namespace x64 {

static const uint32_t MsrSTAR = 0xC0000081;
static const uint32_t MsrLSTAR = 0xC0000082;
static const uint32_t MsrSFMask = 0xC0000084;

void SyscallSetRegisters() {
  AssertCritical();
  uint64_t star = (8L << 0x20) | (0x1bL << 0x30);
  WriteMSR(MsrSTAR, star);
  WriteMSR(MsrSFMask, 0x200); // disable interrupts
  WriteMSR(MsrLSTAR, (uint64_t)RawSyscallHandler);
}

}

}
