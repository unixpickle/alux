#include <arch/x64/acpi/sdt.hpp>
#include <arch/general/failure.hpp>
#include <memory/physcopy.hpp>
#include <cstring>

namespace OS {

namespace x64 {

static SDT * globalSDT;

static RSDP * FindRSDP();
static uint8_t MemChecksum(uint8_t * ptr, int len);

void SDT::Initialize() {
  RSDP * ptr = FindRSDP();
  if (!ptr) {
    Panic("OS::x64::SDT::Initialize() - failed to find RSDP");
  }
  if (ptr->revision == 0) {
    globalSDT = new RSDT((PhysAddr)ptr->ptrRSDT);
  } else {
    globalSDT = new XSDT(ptr->ptrXSDT);
  }
}

SDT & SDT::GetGlobal() {
  return *globalSDT;
}

SDT::SDT(PhysAddr phys) {  
  // read the size field and then map that bugger
  uint32_t size;
  PhysCopy(&size, phys + 4, 4);
  tableSize = size;
  map = new EasyMap(phys, size);
}

SDT::~SDT() {
  delete map;
}

int SDT::FindTable(const char * name) {
  uint32_t num = *((uint32_t *)name);
  for (int i = 0; i < GetCount(); i++) {
    PhysAddr tableAddr = GetTable(i);
    uint32_t typeNum;
    PhysCopy(&typeNum, tableAddr, 4);
    if (typeNum == num) {
      return i;
    }
  }
  return -1;
}

int RSDT::GetCount() {
  return (tableSize - 0x24) / 4;
}

PhysAddr RSDT::GetTable(int i) {
  uint32_t * ptr = (uint32_t *)(map->GetStart() + 0x24);
  return (PhysAddr)ptr[i];
}

int XSDT::GetCount() {
  return (tableSize - 0x24) / 8;
}

PhysAddr XSDT::GetTable(int i) {
  uint64_t * ptr = (uint64_t *)(map->GetStart() + 0x24);
  return ptr[i];
}

static RSDP * FindRSDP() {
  // find the RSDP in the BIOS areas.
  const char * signature = "RSD PTR ";
  uintptr_t ptr;
  
  // the whole potential EBDA
  for (ptr = 0x80000; ptr < 0x9fc00; ptr++) {
    if (memcmp(signature, (void *)ptr, 8) == 0) {
      if (MemChecksum((uint8_t *)ptr, 0x14) != 0) continue;
      return (RSDP *)ptr;
    }
  }
  for (ptr = 0xe0000; ptr < 0x100000; ptr++) {
    if (memcmp(signature, (void *)ptr, 8) == 0) {
      if (MemChecksum((uint8_t *)ptr, 0x14) != 0) continue;
      return (RSDP *)ptr;
    }
  }
  
  return NULL;
}

static uint8_t MemChecksum(uint8_t * ptr, int len) {
  uint8_t sum = 0;
  while (len-- > 0) {
    sum += (*ptr);
    ptr++;
  }
  return sum;
}

}

}
