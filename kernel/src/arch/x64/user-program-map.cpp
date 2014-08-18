#include "user-program-map.hpp"
#include "user-program.hpp"
#include <anarch/api/panic>
#include <anarch/api/domain>
#include <anarch/critical>

namespace OS {

namespace x64 {

UserProgramMap & UserProgramMap::New(anarch::UserMap & m, UserProgram & prog) {
  return *(new UserProgramMap(m, prog));
}

void * UserProgramMap::GetEntryPoint() {
  return (void *)StartAddr;
}

OS::UserProgram & UserProgramMap::GetUserProgram() {
  return program;
}

bool UserProgramMap::HandlePageFault(VirtAddr addr, bool wasWrite) {
  AssertNoncritical();
  anarch::ScopedLock scope(lock);
  
  if (addr < StartAddr || addr >= StartAddr + program.GetLength()) {
    return false;
  }
  Sector & sec = sectors[(addr - StartAddr) / 0x200000];
  if (wasWrite) {
    addr &= ~(PhysAddr)0xfff; // page align it
    HandleWriteFault(sec, addr);
  } else {
    HandleReadFault(sec);
  }
  return true;
}

void UserProgramMap::Delete() {
  AssertNoncritical();
  delete this;
}

UserProgramMap::UserProgramMap(anarch::UserMap & map, UserProgram & prog)
  : OS::UserProgramMap(map), program(prog),
    sectorCount(prog.GetLength() / SectorSize) {
  assert(prog.GetLength() % SectorSize == 0);
  assert(prog.GetMemory() % SectorSize == 0);
  
  // create sector list
  sectors = new Sector[sectorCount];
  for (int i = 0; i < sectorCount; ++i) {
    PhysSize offset = (PhysSize)i * SectorSize;
    sectors[i].startAddr = StartAddr + offset;
    sectors[i].readOnlyStart = prog.GetMemory() + offset;
  }
  
  anarch::UserMap::Size theSize(SectorSize, sectorCount);
  GetMemoryMap().ReserveAt(StartAddr, theSize);
}

UserProgramMap::~UserProgramMap() {
  AssertNoncritical();
  
  // free all of the physical memory and virtual memory used by each sector
  for (int i = 0; i < sectorCount; ++i) {
    if (!sectors[i].writable) continue;
    for (int j = 0; j < 0x200; ++j) {
      PhysAddr writable = sectors[i].writables[j];
      if (!writable) continue;
      anarch::Domain::GetCurrent().FreePhys(writable);
    }
    delete[] sectors[i].writables;
  }
  
  // free the sectors list
  delete[] sectors;
}

void UserProgramMap::HandleReadFault(Sector & sector) {
  if (sector.readable) return;
  sector.readable = true;
  
  anarch::UserMap::Attributes attrs;
  attrs.writable = false;
  
  GetMemoryMap().MapAt(sector.startAddr, sector.readOnlyStart,
                       anarch::UserMap::Size(SectorSize, 1), attrs);
}

void UserProgramMap::HandleFirstWriteFault(Sector & sector,
                                           VirtAddr pageAddr) {
  assert(!sector.writable);
  anarch::UserMap::Size roSize(SectorSize, 1);
  if (sector.readable) {
    GetMemoryMap().UnmapAndReserve(sector.startAddr, roSize);
  }
  GetMemoryMap().Rereserve(sector.startAddr, roSize, 0x1000);
  
  sector.readable = sector.writable = true;
  sector.writables = new PhysAddr[0x200];
  
  // TODO: check if I actually need to do this; it's so unclear man
  for (int i = 0; i < 0x200; ++i) {
    sector.writables[i] = 0;
  }
  
  PhysAddr allocated;
  if (!anarch::Domain::GetCurrent().AllocPhys(allocated, 0x1000, 0x1000)) {
    anarch::Panic("UserProgramMap::HandleFirstWriteFault() - "
                  "Domain::AllocPhys failed");
  }
  
  int pageIdx = (int)((pageAddr % SectorSize) / 0x1000);
  sector.writables[pageIdx] = allocated;
  
  anarch::UserMap::Attributes attrs;
  attrs.writable = false;
  
  // map all the pages as read only
  GetMemoryMap().MapAt(sector.startAddr, sector.readOnlyStart,
                       anarch::UserMap::Size(0x1000, 0x200), attrs);

  attrs.writable = true;
  // map the one writable page
  GetMemoryMap().MapAt(pageAddr, allocated, anarch::UserMap::Size(0x1000, 1),
                       attrs);
}

void UserProgramMap::HandleWriteFault(Sector & sector, VirtAddr pageAddr) {
  if (!sector.writable) {
    return HandleFirstWriteFault(sector, pageAddr);
  }
  
  int pageIdx = (int)((pageAddr % SectorSize) / 0x1000);
  if (sector.writables[pageIdx]) return;
  
  PhysAddr allocated;
  if (!anarch::Domain::GetCurrent().AllocPhys(allocated, 0x1000, 0x1000)) {
    anarch::Panic("UserProgramMap::HandleWriteFault() - "
                  "Domain::AllocPhys failed");
  }
  sector.writables[pageIdx] = allocated;
  
  anarch::UserMap::Attributes attrs;
  GetMemoryMap().MapAt(pageAddr, allocated, anarch::UserMap::Size(0x1000, 1),
                       attrs);
}

}

}