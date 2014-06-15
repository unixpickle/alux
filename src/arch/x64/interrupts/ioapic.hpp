#ifndef __X64_IOAPIC_HPP__
#define __X64_IOAPIC_HPP__

#include <arch/x64/acpi/madt.hpp>
#include <memory/easy-map.hpp>
#include <module/module.hpp>

namespace OS {

namespace x64 {

class IOAPICModule : public Module {
public:
  static void InitGlobal();
  static IOAPICModule & GetGlobal();
  
  virtual void Initialize();
  virtual DepList GetDependencies();
};

class IOAPIC {
public:
  class Entry;
  
  static const int RegVersion = 0x1;
  
  static IOAPIC & GetBase(); // @ambicritical
  static void StartUsing(); // @critical
  
  IOAPIC(); // do not create an IOAPIC this way
  IOAPIC(MADT::IOAPIC * info); // @noncritical
  ~IOAPIC(); // @noncritical
  
  void WriteReg(uint8_t reg, uint32_t val); // @critical
  uint32_t ReadReg(uint8_t reg); // @critical

  uint32_t GetVersion(); // @critical
  uint32_t GetPinCount(); // @critical
  uint32_t GetInterruptBase(); // @critical

  void SetEntry(uint8_t idx, const Entry & entry); // @critical
  void MapIRQ(uint8_t irq, uint8_t vector); // @critical
  void MaskIRQ(uint8_t irq); // @critical
  void MaskPin(uint8_t irq); // @critical
  
  class Entry {
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
  } OS_PACKED;
  
private:
  MADT::IOAPIC madtInfo;
  EasyMap * map;
};

}

}

#endif
