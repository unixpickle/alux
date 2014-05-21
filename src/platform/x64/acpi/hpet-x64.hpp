#ifndef __PLATFORM_X64_HPET_X64_H__
#define __PLATFORM_X64_HPET_X64_H__

#include "rsdp-x64.hpp"
#include <cstring>

namespace OS {

namespace x64 {

namespace ACPI {

class HPET {
public:
  uint32_t signature;
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char oemid[6];
  uint64_t oemTableId;
  uint32_t oemRevision;
  uint32_t creatorId;
  uint32_t creatorRev;
  uint32_t blockID;
  char baseAddr[12];
  uint8_t hpetNumber;
  uint16_t counterMinimum;
  uint8_t pageProtectionAndOEM;
} OS_PACKED;

int GetHPETCount();
HPET GetHPET(int i);

}

}

}

#endif
