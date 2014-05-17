#include "ioapic-x64.hpp"

namespace OS {

namespace x64 {

static IOAPIC baseAPIC;

void IOAPIC::StartUsing() {
  OutB(0x22, 0x70);
  OutB(0x23, 0x01);
}

IOAPIC::IOAPIC(ACPI::MADT::IOAPIC * info) : madtInfo(*info) {
  base = (PhysAddr)madtInfo.baseAddr;
  
  // map in the base address so we can access it from virtual memory
  VirtAddr addr;
  bool res = KernMap::Map(base, 0x1000, addr);
  assert(res);
  regs = (volatile uint32_t *)addr;
  
  cout << "IOAPIC::IOAPIC(" << (uintptr_t)info << ") - base=" << base
    << " version=" << GetVersion() << " pinCount=" << GetPinCount() << endl;
}

IOAPIC::~IOAPIC() {
  // unmap the address here
  Panic("Destroying an IOAPIC will result in a map leak.");
  // KernMap::Unmap((VirtAddr)regs, 0x1000);
}

void IOAPIC::WriteReg(uint8_t reg, uint32_t val) {
  regs[0] = reg;
  regs[4] = val;
}

uint32_t IOAPIC::ReadReg(uint8_t reg) {
  regs[0] = reg;
  return regs[4];
}

uint32_t IOAPIC::GetVersion() {
  return ReadReg(RegVersion) & 0xff;
}

uint32_t IOAPIC::GetPinCount() {
  return 1 + ((ReadReg(RegVersion) >> 0x10) & 0xff);
}

uint32_t IOAPIC::GetInterruptBase() {
  return madtInfo.interruptBase;
}

void IOAPIC::SetTable(uint8_t idx, const TableEntry & entry) {
  const uint32_t * valPtr = (const uint32_t *)&entry;
  WriteReg(0x10 + (idx * 2), 0x10000); // mask the entry
  WriteReg(0x11 + (idx * 2), valPtr[1]);
  WriteReg(0x10 + (idx * 2), valPtr[0]); 
}

void IOAPIC::MapIRQ(uint8_t irq, uint8_t vector) {
  TableEntry entry;
  entry.vector = vector;
  
  ACPI::MADT::ISO * iso = ACPI::GetMADT()->LookupISO(irq);
  if (iso) {
    if (iso->flags & 0x3) entry.intpol = 1;
    if ((iso->flags >> 2) & 0x3) entry.triggermode = 1;
    SetTable(iso->interrupt, entry);
  } else {
    SetTable(irq, entry);
  }
}

void IOAPIC::MaskPin(uint8_t irq) {
  WriteReg(0x10 + (irq * 2), 0x10000);
}

void InitializeIOAPIC() {
  ACPI::MADT * madt = ACPI::GetMADT();
  if (!madt) Panic("I/O APIC support requires MADT");
  cout << "MADT info: IOAPICs=" << madt->CountIOAPICEntries() << " LAPICS="
    << madt->CountLocalAPICEntries() << endl;
  
  ACPI::MADT::IOAPIC * info = madt->GetIOAPICWithBase(0);
  if (!info) Panic("No base I/O APIC found :(");
  
  new(&baseAPIC) IOAPIC(info);
}

IOAPIC & GetBaseIOAPIC() {
  return baseAPIC;
}

}

}
