#include <arch/x64/smp/startup-code.hpp>
#include <cassert>
#include <cstring>

namespace OS {

namespace x64 {

static const uint16_t codeAddress = 0x5000;

StartupCode::StartupCode() {
  const char * buffer = (const char *)0x100000;
  const char * codeStart = NULL;
  uint64_t codeLength;
  for (int i = 0; i < 0x1000; i++) {
    if (!memcmp(buffer + i, "_X64_PROC_ENTRY_", 0x10)) {
      codeStart = buffer + i + 0x18;
      memcpy(&codeLength, buffer + i + 0x10, 8);
    }
  }
  
  assert(codeStart != NULL);

  codeSize = codeLength;
  backup = (void *)(new uint8_t[codeSize + 0x1000]);
  memcpy(backup, (void *)(codeAddress - 0x1000), codeSize + 0x1000);

  memcpy((void *)codeAddress, codeStart, codeSize);
}

StartupCode::~StartupCode() {
  memcpy((void *)(codeAddress - 0x1000), backup, codeSize + 0x1000);
  delete (uint8_t *)backup;
}

void StartupCode::UpdateStack(uint64_t pml4, uint64_t funcAddr) {
  uint64_t * buffer = (uint64_t *)(codeAddress - 0x10);
  buffer[0] = pml4;
  buffer[1] = funcAddr;
}

uint8_t StartupCode::GetStartupVector() {
  return codeAddress / 0x1000;
}

}

}
