#include <arch/x64/interrupts/ioapic.hpp>
#include <arch/general/failure.hpp>
#include <arch/x64/common.hpp>
#include <iostream>
#include <cstring>
#include <new>

namespace OS {

namespace x64 {

static IOAPIC baseAPIC;

void IOAPIC::Initialize() {
  MADT * madt = MADT::GetGlobal();
  if (!madt) Panic("I/O APIC support requires MADT");
  
  cout << "MADT info: IOAPICs=" << madt->CountIOAPICs() << " LAPICS="
    << madt->CountLAPICs(false) << endl;
  
  MADT::IOAPIC * info = madt->LookupIOAPIC(0);
  if (!info) Panic("No base I/O APIC found");
  
  new(&baseAPIC) IOAPIC(info);
}

IOAPIC & IOAPIC::GetBase() {
  return baseAPIC;
}

void IOAPIC::StartUsing() {
  OutB(0x22, 0x70);
  OutB(0x23, 0x01);
}

IOAPIC::IOAPIC() {
  Panic("IOAPIC::IOAPIC() - this method is for the compiler only");
}

IOAPIC::IOAPIC(MADT::IOAPIC * info) : madtInfo(*info) {
  map = new EasyMap(madtInfo.baseAddr, 0x1000);
  cout << "IOAPIC::IOAPIC(" << (uintptr_t)info << ") - base="
    << madtInfo.baseAddr << " version=" << GetVersion() << " pinCount="
    << GetPinCount() << endl;
}

IOAPIC::~IOAPIC() {
  delete map;
}

void IOAPIC::WriteReg(uint8_t reg, uint32_t val) {
  volatile uint32_t * regs = (volatile uint32_t *)map->GetStart();
  regs[0] = reg;
  regs[4] = val;
}

uint32_t IOAPIC::ReadReg(uint8_t reg) {
  volatile uint32_t * regs = (volatile uint32_t *)map->GetStart();
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

void IOAPIC::SetEntry(uint8_t idx, const Entry & entry) {
  const uint32_t * valPtr = (const uint32_t *)&entry;
  WriteReg(0x10 + (idx * 2), 0x10000); // mask the entry
  WriteReg(0x11 + (idx * 2), valPtr[1]);
  WriteReg(0x10 + (idx * 2), valPtr[0]); 
}

void IOAPIC::MapIRQ(uint8_t irq, uint8_t vector) {
  Entry entry;
  bzero(&entry, sizeof(entry));
  entry.vector = vector;

  MADT::ISO * iso = MADT::GetGlobal()->LookupISO(irq);
  if (iso) {
    // TODO: idk why i do & 0x3, i should check this out
    if (iso->flags & 0x3) entry.intpol = 1;
    if ((iso->flags >> 2) & 0x3) entry.triggermode = 1;
    SetEntry(iso->interrupt, entry);
  } else {
    SetEntry(irq, entry);
  }
}

void IOAPIC::MaskIRQ(uint8_t irq) {
  MADT::ISO * iso = MADT::GetGlobal()->LookupISO(irq);
  if (iso) {
    MaskPin(iso->interrupt);
  } else {
    MaskPin(irq);
  }
}

void IOAPIC::MaskPin(uint8_t irq) {
  WriteReg(0x10 + (irq * 2), 0x10000);
}

}

}
