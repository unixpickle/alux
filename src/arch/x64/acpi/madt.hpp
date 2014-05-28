#ifndef __X64_MADT_HPP__
#define __X64_MADT_HPP__

#include <memory/easy-map.hpp>
#include <common>

namespace OS {

namespace x64 {

class MADT {
public:
  class Header {
  public:
    uint32_t signature; // APIC
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oemid[6];
    uint64_t oemTableId;
    uint32_t oemRevision;
    uint32_t creatorId;
    uint32_t creatorRevision;
    uint32_t lapicAddr;
    uint32_t flags;
  } OS_PACKED;
  
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
  
  static const uint8_t TypeLAPIC = 0;
  static const uint8_t TypeIOAPIC = 1;
  static const uint8_t TypeISO = 2;
  static const uint8_t TypeX2APIC = 9;
  
  static void Initialize();
  static MADT * GetGlobal();
  
  MADT(PhysAddr phyPtr);
  ~MADT();
  
  const Header & GetHeader();
  
  int GetTableCount();
  uint8_t * GetTable(int i);
  
  bool SystemHas8259();
  int CountType(uint8_t type);
  int CountIOAPICs();
  int CountLAPICs(bool checkUsable);
  
  ISO * LookupISO(uint8_t physIRQ);
  IOAPIC * LookupIOAPIC(uint32_t base);

protected:
  EasyMap * map;
  int tableCount;
  
  uint8_t * GetData();
  size_t GetDataSize();
};

}

}

#endif
