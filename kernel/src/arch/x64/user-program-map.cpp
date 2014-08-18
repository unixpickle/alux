#include "user-program-map.hpp"
#include "user-program.hpp"
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
  : OS::UserProgramMap(map), program(prog) {
  assert(prog.GetLength() % Sector::Size == 0);
  assert(prog.GetMemory() % Sector::Size == 0);
  PhysSize sectorCount = prog.GetLength() / 0x200000;
  sectors = new[] Sector[sectorCount];
  GetMemoryMap().ReserveAt(StartAddr,
                           anarch::UserMap::Size(Sector::Size, sectorCount));
}

UserProgramMap::~UserProgramMap() {
  // TODO: here, delete physical pages that we allocated
  delete[] sectors;
}

void UserProgramMap::HandleReadFault(Sector & sector) {
  if (sector.readable) return;
  sector.readable = true;
  
  PhysAddr start = program.GetMemory() + Sector::Size * GetSectorIndex(sector);
  VirtAddr dest = StartAddr + Sector::Size * GetSectorIndex(sector);
  anarch::UserMap::Attributes attrs;
  attrs.writable = false;
  
  GetMemoryMap().MapAt(dest, start, anarch::UserMap::Size(Sector::Size, 1),
                       attrs);
}

void UserProgramMap::HandleWriteFault(Sector & sector, VirtAddr addr) {
  if (sector.writable) return;
  if (sector.readable) {
    // TODO: some way to go from big pages to small pages
  }
  sector.readable = sector.writable = true;
  sector.writables = new[] PhysAddr[0x200];
  
  // TODO: check if i actually need to do this; it's so unclear man
  for (int i = 0; i < 0x200; ++i) {
    sector.writables[i] = 0;
  }
  
  PhysAddr allocated;
  // TODO: create Domain physical memory allocation!
#warning THIS IS NOT DONE
  
  int pageIdx = (int)((addr % Sector::Size) / 0x1000);
  sector.writables[pageIdx] = allocated;
}

size_t UserProgramMap::GetSectorIndex(Sector & s) {
  return (&s - sectors) / sizeof(Sector);
}

}

}