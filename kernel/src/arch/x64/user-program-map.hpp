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
  
  static UserProgramMap & New(anarch::UserMap & map, UserProgram & prog);
  
  virtual void * GetEntryPoint();
  virtual OS::UserProgram & GetUserProgram();
  
  virtual bool HandlePageFault(VirtAddr addr, bool write);
  virtual void Delete();
  
private:
  UserProgramMap(anarch::UserMap & map, UserProgram & prog);
  virtual ~UserProgramMap();
  
  void HandleReadFault(Sector & sector);
  void HandleWriteFault(Sector & sector, VirtAddr addr);
  size_t GetSectorIndex(Sector &);
  
  UserProgram & program;
  
  /**
   * A 2MB sector.
   */
  struct Sector {
    static const PhysSize Size = 0x200000;
    
    bool readable = false;
    bool writable = false;
    PhysAddr * writables = NULL;
  };
  
  anarch::NoncriticalLock lock;
  Sector * sectors;
};

}

}

#endif
