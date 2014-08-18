#ifndef __ALUX_X64_USER_PROGRAM_MAP_HPP__
#define __ALUX_X64_USER_PROGRAM_MAP_HPP__

#include "../all/user-program-map.hpp"
#include <anarch/lock>

namespace OS {

namespace x64 {

class UserProgram;

class UserProgramMap : public OS::UserProgramMap {
public:
  static const VirtAddr StartAddr = 0x8000000000UL;
  static const PhysSize SectorSize = 0x200000;
  
  static UserProgramMap & New(anarch::UserMap & map, UserProgram & prog);
  
  virtual void * GetEntryPoint();
  virtual OS::UserProgram & GetUserProgram();
  
  virtual bool HandlePageFault(VirtAddr addr, bool write);
  virtual void Delete();
  
private:
  /**
   * A 2MB sector.
   */
  struct Sector {
    bool readable = false;
    bool writable = false;
    PhysAddr * writables = NULL;
    VirtAddr startAddr;
    PhysAddr readOnlyStart;
  };
  
  UserProgramMap(anarch::UserMap & map, UserProgram & prog);
  virtual ~UserProgramMap();
  
  void HandleReadFault(Sector & sector);
  void HandleFirstWriteFault(Sector & sector, VirtAddr pageAddr);
  void HandleWriteFault(Sector & sector, VirtAddr pageAddr);
  
  UserProgram & program;
  
  anarch::NoncriticalLock lock;
  Sector * sectors;
  int sectorCount;
};

}

}

#endif
