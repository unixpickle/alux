#ifndef __PLATFORM_X64_RSDP_X64_H__
#define __PLATFORM_X64_RSDP_X64_H__

#include <cstdint>
#include <utilities/common.hpp>
#include <platform/failure.hpp>
#include "../memory/phys-memcpy-x64.hpp"

namespace OS {

namespace x64 {

namespace ACPI {

class RSDP {
public:
  uint64_t signature;
  uint8_t checksum;
  char oemid[6];
  uint8_t revision;
  uint32_t ptrRSDT;
  uint32_t length;
  uint64_t ptrXSDT;
  uint8_t xChecksum;
  char reserved[3];
  
  uint64_t TableCount();
  PhysAddr GetTable(uint64_t idx);
} OS_PACKED;

RSDP & GetRSDP();

}

}

}

#endif
