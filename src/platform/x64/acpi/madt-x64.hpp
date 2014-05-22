#ifndef __PLATFORM_X64_MADT_X64_H__
#define __PLATFORM_X64_MADT_X64_H__

#include "rsdp-x64.hpp"
#include "acpi-structures-x64.hpp"
#include <cstring>
#include <new>

namespace OS {
namespace x64 {
namespace ACPI {

class MADTHeader : public Table {
public:
  uint32_t lapicAddr;
  uint32_t flags;
} OS_PACKED;

class MADT {
private:
  MADTHeader header;
  uint8_t * data;
  size_t dataSize;
  
  int tableCount;
  
public:
  static const size_t HeaderSize = sizeof(MADTHeader);
  static const uint8_t TypeLAPIC = 0;
  static const uint8_t TypeIOAPIC = 1;
  static const uint8_t TypeISO = 2;
  static const uint8_t Typex2APIC = 9;
  
  class ISO {
  public:
    uint8_t type;
    uint8_t length;
    uint8_t bus;
    uint8_t source;
    uint32_t interrupt;
    uint16_t flags;
  } OS_PACKED;
  
  class LocalAPIC {
  public:
    uint8_t type; // should be 0
    uint8_t length;
    uint8_t apicpuId;
    uint8_t apicId;
    uint32_t flags; // bit 0 set = usable
  } OS_PACKED;
  
  class LocalAPIC2 {
  public:
    uint8_t type; // should be 9
    uint8_t length;
    uint16_t reserved;
    uint32_t x2apicId;
    uint32_t flags; // bit 0 set = usable
    uint32_t x2apicpuId;
  } OS_PACKED;
  
  class IOAPIC {
  public:
    uint8_t type;
    uint8_t length;
    uint8_t identifier;
    uint8_t reserved;
    uint32_t baseAddr;
    uint32_t interruptBase;
  } OS_PACKED;
  
  MADT() {}
  MADT(PhysAddr phyPtr);
  ~MADT();
  
  const MADTHeader & GetHeader();
  const void * GetData();
  size_t GetDataSize();
  
  int GetTableCount();
  uint8_t * GetTable(int i);
  
  // feature information
  bool SystemHas8259();
  int CountType(uint8_t type);
  int CountIOAPICEntries();
  int CountLocalAPICEntries(bool checkUsable = false);
  
  ISO * LookupISO(uint8_t physicalIRQ);
  IOAPIC * GetIOAPICWithBase(uint32_t base);
};

/**
 * Gets the multiprocessing table descriptor, or returns NULL if none exists.
 */
MADT * GetMADT();

}
}
}

#endif
