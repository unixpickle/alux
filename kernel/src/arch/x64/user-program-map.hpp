#ifndef __ALUX_X64_USER_PROGRAM_MAP_HPP__
#define __ALUX_X64_USER_PROGRAM_MAP_HPP__

#include "../all/user-program-map.hpp"

namespace OS {

namespace x64 {

class UserProgram;

class UserProgramMap : public OS::UserProgramMap {
public:
  static UserProgramMap & New(UserProgram & prog);
  
  virtual void * GetEntryPoint();
  virtual OS::UserProgram & GetUserProgram();
  
  virtual bool HandlePageFault(VirtAddr addr, bool write);
  virtual void Delete();
  
private:
  UserProgramMap(UserProgram & prog);
  
  UserProgram & program;
};

}

}

#endif
