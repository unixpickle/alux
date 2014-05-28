#include <arch/x64/acpi/madt.hpp>
#include <arch/x64/acpi/sdt.hpp>
#include <memory/physcopy.hpp>

namespace OS {

namespace x64 {

static MADT * globalMADT = NULL;

void MADT::Initialize() {
  SDT & sdt = SDT::GetGlobal();
  int idx = sdt.FindTable("APIC");
  if (idx < 0) return;
  PhysAddr addr = sdt.GetTable(idx);
  globalMADT = new MADT(addr);
}

MADT * MADT::GetGlobal() {
  return globalMADT;
}

MADT::MADT(PhysAddr phyPtr) : tableCount(0) {
  uint32_t size;
  PhysCopy(&size, phyPtr + 4, 4);
  map = new EasyMap(phyPtr, size, false);
  
  size_t offset = 1;
  while (offset < GetDataSize()) {
    uint8_t thisSize = GetData()[offset];
    offset += thisSize;
    tableCount++;
  }
}

MADT::~MADT() {
  delete map;
}

const MADT::Header & MADT::GetHeader() {
  Header * headerPtr = (Header *)map->GetStart();
  return *headerPtr;
}

int MADT::GetTableCount() {
  return tableCount;
}

uint8_t * MADT::GetTable(int i) {
  size_t offset = 0;
  int count = 0;
  while (offset + 1 < GetDataSize()) {
    uint8_t * body = GetData() + offset;
    if (count == i) return body;
    offset += body[1];
    count++;
  }
  return NULL;
}

bool MADT::SystemHas8259() {
  return GetHeader().flags & 1;
}

int MADT::CountType(uint8_t type) {
  int count = 0;
  size_t offset = 0;
  for (int i = 0; i < tableCount; i++) {
    if (GetData()[offset] == type) count++;
    offset += GetData()[1 + offset];
  }
  return count;
}

int MADT::CountIOAPICs() {
  return CountType(TypeIOAPIC);
}

int MADT::CountLAPICs(bool checkUsable) {
  if (!checkUsable) {
    return CountType(TypeLAPIC) + CountType(TypeX2APIC);
  } else {
    int count = 0;
    size_t offset = 0;
    for (int i = 0; i < tableCount; i++) {
      if (GetData()[offset] == TypeLAPIC) {
        LocalAPIC * apic = (LocalAPIC *)&GetData()[offset];
        if (apic->flags & 1) count++;
      } else if (GetData()[offset] == TypeX2APIC) {
        LocalAPIC2 * apic = (LocalAPIC2 *)&GetData()[offset];
        if (apic->flags & 1) count++;
      }
      offset += GetData()[1 + offset];
    }
    return count;
  }
}

MADT::ISO * MADT::LookupISO(uint8_t physIRQ) {
  size_t offset = 0;
  for (int i = 0; i < tableCount; i++) {
    if (GetData()[offset] == TypeISO) {
      ISO * iso = (ISO *)&GetData()[offset];
      if (iso->source == physIRQ && iso->bus == 0) {
        return iso;
      }
    }
    offset += GetData()[1 + offset];
  }
  return NULL;
}

MADT::IOAPIC * MADT::LookupIOAPIC(uint32_t base) {
  size_t offset = 0;
  for (int i = 0; i < tableCount; i++) {
    if (GetData()[offset] == TypeIOAPIC) {
      IOAPIC * info = (IOAPIC *)&GetData()[offset];
      if (info->interruptBase == base) return info;
    }
    offset += GetData()[1 + offset];
  }
  return NULL;
}

uint8_t * MADT::GetData() {
  return (uint8_t *)(map->GetStart() + sizeof(Header));
}

size_t MADT::GetDataSize() {
  return (size_t)GetHeader().length - sizeof(Header);
}

}

}
