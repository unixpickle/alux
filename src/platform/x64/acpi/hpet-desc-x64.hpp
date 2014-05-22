#ifndef __PLATFORM_X64_HPET_X64_H__
#define __PLATFORM_X64_HPET_X64_H__

#include "rsdp-x64.hpp"
#include "acpi-structures-x64.hpp"
#include <cstring>

namespace OS {
namespace x64 {
namespace ACPI {

class HPETDesc : public Table {
public:
  uint32_t blockID;
  char baseAddr[12];
  uint8_t hpetNumber;
  uint16_t counterMinimum;
  uint8_t pageProtectionAndOEM;
} OS_PACKED;

int GetHPETCount();
HPETDesc GetHPET(int i);

}
}
}

#endif
