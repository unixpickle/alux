#ifndef __ALUX_X64_EXECUTABLE_MAP_HPP__
#define __ALUX_X64_EXECUTABLE_MAP_HPP__

#include "../all/executable-map.hpp"
#include <anarch/lock>

namespace Alux {

namespace x64 {

class Executable;

class ExecutableMap : public Alux::ExecutableMap {
public:
  static const VirtAddr StartAddr = 0x8000000000UL;
  
  static ExecutableMap & New(Executable &, anarch::UserMap &);
  
  virtual Alux::Executable & GetExecutable();
  virtual void * GetEntryPoint();
  virtual bool HandlePageFault(VirtAddr addr, bool write);
  virtual void Delete();
  
private:
  ExecutableMap(Executable &, anarch::UserMap &);
  virtual ~ExecutableMap();
  
  Executable & executable;
  
  struct Sector {
    int mode = 0; // 0 = unmapped, 1 = read, 2 = read/write
    PhysAddr * writables = NULL;
    VirtAddr virtualAddr;
    PhysAddr readOnlyAddr;
  };
  
  void HandleReadFault(Sector &, VirtAddr pageAddr);
  void HandleWriteFault(Sector &, VirtAddr pageAddr);
  
  void MapROLargePage(Sector &);
  void MapROSmallPage(Sector &, VirtAddr pageAddr);
  void SwitchToWritable(Sector &);
  void MapWritablePage(Sector &, VirtAddr pageAddr);
  
  anarch::NoncriticalLock lock;
  int sectorCount;
  Sector * sectors;
};

}

}

#endif
