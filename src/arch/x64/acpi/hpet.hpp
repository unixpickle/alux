#ifndef __X64_ACPI_HPET__
#define __X64_ACPI_HPET__

#include <cstdint>
#include <macros>

namespace OS {

namespace x64 {

class HPETInfo {
public:
  class Address {
  public:
    uint8_t addressSpaceId;
    uint8_t registerBitWidth;
    uint8_t registerBitOffset;
    uint8_t reserved;
    uint64_t address;
  } OS_PACKED;
  
  static HPETInfo * GetGlobal(); // @noncritical
  
  uint32_t signature; // HPET
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char oemid[6];
  uint64_t oemTableId;
  uint32_t oemRevision;
  uint32_t creatorId;
  uint32_t creatorRevision;
    
  uint32_t blockID;
  Address address;
  uint8_t hpetNumber;
  uint16_t counterMinimum;
  uint8_t pageProtectionAndOEM;
} OS_PACKED;

}

}

#endif
