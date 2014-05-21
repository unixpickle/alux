#include "rsdp-x64.hpp"
#include <iostream> // todo: comment this out

namespace OS {

namespace x64 {

namespace ACPI {

static RSDP * rsdp = NULL;

static bool FindRSDP();
static uint8_t MemChecksum(uint8_t * ptr, int len);

RSDP & GetRSDP() {
  if (!rsdp) {
    if (!FindRSDP()) {
      Panic("OS::x64::ACPI::GetRSDP() - not found");
    }
    for (int i = 0; i < rsdp->TableCount(); i++) {
      auto buf = rsdp->GetTable(i);
      char dest[5] = {0};
      MemcpyToVirt((VirtAddr)dest, buf, 4);
      cout << "name is " << dest << " at " << i << endl;
    }
  }
  return *rsdp;
}

uint64_t RSDP::TableCount() {
  if (revision == 0) {
    // use RSDT
    uint32_t lenField = 0;
    uint64_t offset = (uint64_t)ptrRSDT;
    MemcpyToVirt((VirtAddr)&lenField, (PhysAddr)(offset + 4), 4);
    return (lenField - 0x24) >> 2;
  } else {
    // use XSDT    
    uint32_t lenField = 0;
    uint64_t offset = ptrXSDT;
    MemcpyToVirt((VirtAddr)&lenField, (PhysAddr)(offset + 4), 4);
    return (lenField - 0x24) >> 3;
  }
}

PhysAddr RSDP::GetTable(uint64_t idx) {
  if (rsdp->revision == 0) {
    // use RSDT
    uint64_t off = 0x24 + (idx << 2);
    uint64_t res = 0; // must initialize to 0!
    uint64_t offset = (uint64_t)ptrRSDT + off;
    MemcpyToVirt((VirtAddr)&res, (PhysAddr)offset, 4);
    return res;
  } else {
    // use XSDT
    uint64_t off = 0x24 + (idx << 3);
    uint64_t res;
    MemcpyToVirt((VirtAddr)&res, (PhysAddr)(ptrXSDT + off), 8);
    return res;
  }
}

/***********
 * Private *
 ***********/

static bool FindRSDP() {
  // find the RSDP in the BIOS areas.
  const char * signature = "RSD PTR ";
  uintptr_t ptr;
  
  // the whole potential EBDA
  for (ptr = 0x80000; ptr < 0x9fc00; ptr++) {
    if (memcmp(signature, (void *)ptr, 8) == 0) {
      if (MemChecksum((uint8_t *)ptr, 0x14) != 0) continue;
      rsdp = (RSDP *)ptr;
      return true;
    }
  }
  for (ptr = 0xe0000; ptr < 0x100000; ptr++) {
    if (memcmp(signature, (void *)ptr, 8) == 0) {
      if (MemChecksum((uint8_t *)ptr, 0x14) != 0) continue;
      rsdp = (RSDP *)ptr;
      return true;
    }
  }
  
  return false;
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

}
