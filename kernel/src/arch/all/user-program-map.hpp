#ifndef __ALUX_USER_PROGRAM_MAP_HPP__
#define __ALUX_USER_PROGRAM_MAP_HPP__

#include <anarch/types>

namespace OS {

class UserProgram;

class UserProgramMap {
public:
  virtual void * GetEntryPoint() = 0; // @ambicritical
  virtual UserProgram & GetUserProgram() = 0; // @ambicritical
  
  virtual bool HandlePageFault(VirtAddr addr, bool write) = 0; // @noncritical
  virtual void Delete() = 0; // @noncritical
  
protected:
  virtual ~UserProgramMap() {}
};

}

#endif
