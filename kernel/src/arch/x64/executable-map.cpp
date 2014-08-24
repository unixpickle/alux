#include "executable-map.hpp"
#include "executable.hpp"
#include <anarch/api/domain>
#include <anarch/api/panic>
#include <anarch/critical>
#include <ansa/cstring>

namespace Alux {

namespace x64 {

ExecutableMap & ExecutableMap::New(Executable & e, anarch::UserMap & m) {
  ExecutableMap * res = new ExecutableMap(e, m);
  assert(res != NULL);
  return *res;
}

Alux::Executable & ExecutableMap::GetExecutable() {
  return executable;
}

void * ExecutableMap::GetEntryPoint() {
  return (void *)StartAddr;
}

bool ExecutableMap::HandlePageFault(VirtAddr addr, bool write) {
  AssertNoncritical();
  anarch::ScopedLock scope(lock);

  if (addr < StartAddr || addr >= StartAddr + executable.GetLength()) {
    return false;
  }

  int idx = (int)((addr - StartAddr) / 0x200000);
  assert(idx >= 0 && idx < sectorCount);
  Sector & sector = sectors[idx];
  addr &= ~(PhysAddr)0xfff; // page align it
  if (write) {
    HandleWriteFault(sector, addr);
  } else {
    HandleReadFault(sector, addr);
  }
  
  return true;
}

void ExecutableMap::Delete() {
  AssertNoncritical();
  delete this;
}

ExecutableMap::ExecutableMap(Executable & e, anarch::UserMap & m)
  : Alux::ExecutableMap(m), executable(e) {
  sectorCount = executable.GetLength() / 0x200000;
  if (!sectorCount) return;
  
  sectors = new Sector[sectorCount];
  assert(sectors != NULL);
  for (int i = 0; i < sectorCount; ++i) {
    size_t offset = (size_t)i * 0x200000;
    sectors[i].virtualAddr = StartAddr + offset;
    sectors[i].readOnlyAddr = executable.GetMemory() + offset;
  }
  
  anarch::UserMap::Size size(0x200000, sectorCount);
  GetMap().ReserveAt(StartAddr, size);
}

ExecutableMap::~ExecutableMap() {
  for (int i = 0; i < sectorCount; ++i) {
    if (sectors[i].mode != 2) continue;
    // free each writable region of physical memory
    for (int j = 0; j < 0x200; ++j) {
      PhysAddr writable = sectors[i].writables[j];
      if (!writable) continue;
      anarch::Domain::GetCurrent().FreePhys(writable);
    }
    // free the writables list
    delete[] sectors[i].writables;
  }
  
  // free the sector list
  delete[] sectors;
}

void ExecutableMap::HandleReadFault(Sector & s, VirtAddr pageAddr) {
  // if mode is 1, then they shouldn't get this fault if they try again
  if (s.mode == 0) {
    MapROLargePage(s);
  } else if (s.mode == 2) {
    MapROSmallPage(s, pageAddr);
  }
}

void ExecutableMap::HandleWriteFault(Sector & sector, VirtAddr pageAddr) {
  if (sector.mode != 2) {
    SwitchToWritable(sector);
  }
  MapWritablePage(sector, pageAddr);
}

void ExecutableMap::MapROLargePage(Sector & sector) {
  anarch::UserMap::Attributes attrs;
  attrs.writable = false;
  
  GetMap().MapAt(sector.virtualAddr, sector.readOnlyAddr,
                 anarch::UserMap::Size(0x200000, 1), attrs);
  
  sector.mode = 1;
}

void ExecutableMap::MapROSmallPage(Sector & sector, VirtAddr pageAddr) {
  PhysSize subIndex = pageAddr % 0x200000;
  
  anarch::UserMap::Attributes attrs;
  attrs.writable = false;

  GetMap().MapAt(sector.virtualAddr + subIndex, sector.readOnlyAddr + subIndex,
                 anarch::UserMap::Size(0x1000, 1), attrs);
}

void ExecutableMap::SwitchToWritable(Sector & sector) {
  anarch::UserMap::Size bigSize(0x200000, 1);
  if (sector.mode == 1) {
    GetMap().UnmapAndReserve(sector.virtualAddr, bigSize);
  }
  GetMap().Rereserve(sector.virtualAddr, bigSize, 0x1000);
  
  sector.writables = new PhysAddr[0x200]();
  sector.mode = 2;
}

void ExecutableMap::MapWritablePage(Sector & sector, VirtAddr pageAddr) {
  PhysAddr page;
  if (!anarch::Domain::GetCurrent().AllocPhys(page, 0x1000, 0x1000)) {
    anarch::Panic("ExecutableMap::MapWritablePage() - alloc failed");
  }

  int pageIdx = (int)((pageAddr % 0x200000) / 0x1000);
  sector.writables[pageIdx] = page;

  anarch::UserMap::Attributes attrs;
  GetMap().MapAt(pageAddr, page, anarch::UserMap::Size(0x1000, 1), attrs);
  
  // copy the read-only memory into the writable memory
  uint8_t * virtualSource = (uint8_t *)executable.GetReadableMemory() +
    (pageAddr - StartAddr);
  ansa::Memcpy((void *)pageAddr, (void *)virtualSource, 0x1000);
}

}

}
