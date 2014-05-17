#ifndef __PLATFORM_X64_IOAPIC_X64_H__
#define __PLATFORM_X64_IOAPIC_X64_H__

#include "../acpi/madt-x64.hpp"
#include "../common-x64.hpp"
#include <platform/failure.hpp>
#include <platform/memory.hpp>

namespace OS {

namespace x64 {

class IOAPIC {
private:
  PhysAddr base;
  ACPI::MADT::IOAPIC madtInfo;
  
  volatile uint32_t * regs;
  
public:
  static const int RegVersion = 0x1;
  
  static void StartUsing();
  
  class TableEntry {
  public:
    unsigned vector : 8; // RW - processor register
    unsigned delmode : 3; // RW
    unsigned destmode : 1; // RW - determines type for destfield
    unsigned delstatus : 1; // RO
    unsigned intpol : 1; // RW - 0 = high active, 1 = low active
    unsigned remirr : 1; // RO
    unsigned triggermode : 1; // 1 = level sensitive, 0 = edge sensitive
    unsigned imask : 1; // 1 = prevent this interrupt
    unsigned long long reserved : 39; // set this to 0
    unsigned destfield : 8; // RW - APIC ID or "set of processors"
    
    TableEntry() : vector(0), delmode(0), destmode(0), delstatus(0), intpol(0),
      remirr(0), triggermode(0), imask(0), reserved(0), destfield(0) {}
  } OS_PACKED;
  
  IOAPIC() {
    Panic("This is only for the compiler's satisfaction!");
  }
  
  IOAPIC(ACPI::MADT::IOAPIC * _info);
  ~IOAPIC();
  
  void WriteReg(uint8_t reg, uint32_t val);
  uint32_t ReadReg(uint8_t reg);
  
  uint32_t GetVersion();
  uint32_t GetPinCount();
  uint32_t GetInterruptBase();
  
  void SetTable(uint8_t idx, const TableEntry & entry);
  void MapIRQ(uint8_t irq, uint8_t vector);
  void MaskPin(uint8_t irq);
};

void InitializeIOAPIC();
IOAPIC & GetBaseIOAPIC();

}

}

#endif
