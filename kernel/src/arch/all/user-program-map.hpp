#ifndef __ALUX_USER_PROGRAM_MAP_HPP__
#define __ALUX_USER_PROGRAM_MAP_HPP__

#include <anarch/types>
#include <anarch/api/user-map>

namespace OS {

class UserProgram;

class UserProgramMap {
public:
  virtual void * GetEntryPoint() = 0; // @ambicritical
  virtual UserProgram & GetUserProgram() = 0; // @ambicritical
  
  virtual bool HandlePageFault(VirtAddr addr, bool write) = 0; // @noncritical
  virtual void Delete() = 0; // @noncritical
  
  anarch::UserMap & GetMemoryMap() const {
    return memoryMap;
  }
  
protected:
  UserProgramMap(anarch::UserMap & map) : memoryMap(map) {}
  virtual ~UserProgramMap() {}
  
  anarch::UserMap & memoryMap;
};

}

#endif
