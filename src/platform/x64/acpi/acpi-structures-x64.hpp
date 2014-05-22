#ifndef __PLATFORM_X64_ACPI_STRUCTURES_HPP__
#define __PLATFORM_X64_ACPI_STRUCTURES_HPP__

#include <cstdint>
#include <utilities/common.hpp>

namespace OS {
namespace x64 {
namespace ACPI {

class Table {
public:
  uint32_t signature; // HPET, ACPI, etc.
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char oemid[6];
  uint64_t oemTableId;
  uint32_t oemRevision;
  uint32_t creatorId;
  uint32_t creatorRevision;
} OS_PACKED;

}
}
}

#endif

