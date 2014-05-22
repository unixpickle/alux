#include "hpet-desc-x64.hpp"

namespace OS {
namespace x64 {
namespace ACPI {

int GetHPETCount() {
  RSDP & rsdp = GetRSDP();
  uint64_t count = 0;
  for (uint64_t i = 0; i < rsdp.TableCount(); i++) {
    char name[4];
    MemcpyToVirt((VirtAddr)name, rsdp.GetTable(i), 4);
    if (!memcmp(name, "HPET", 4)) {
      count++;
    }
  }
  return (int)count;
}

HPETDesc GetHPET(int idx) {
  HPETDesc h;
  RSDP & rsdp = GetRSDP();
  uint64_t count = 0;
  for (uint64_t i = 0; i < rsdp.TableCount(); i++) {
    char name[4];
    MemcpyToVirt((VirtAddr)name, rsdp.GetTable(i), 4);
    if (!memcmp(name, "HPET", 4)) {
      if (count == (uint64_t)idx) {
        MemcpyToVirt((VirtAddr)&h, rsdp.GetTable(i), sizeof(h));
        return h;
      }
      count++;
    }
  }
  Panic("HPET index out of bounds");
  return h;
}

}
}
}
