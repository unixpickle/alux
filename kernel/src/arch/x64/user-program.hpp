#ifndef __ALUX_X64_USER_PROGRAM_HPP__
#define __ALUX_X64_USER_PROGRAM_HPP__

#include "../all/user-program.hpp"
#include "user-program-map.hpp"

namespace OS {

namespace x64 {

class UserProgram : public OS::UserProgram {
public:
  /**
   * Construct a user-program from a memory address. The memory must be 2MB
   * aligned so that large TLB pages may be used. Additionally, `length` must
   * be an integer multiple of 2MB.
   */
  UserProgram(PhysAddr memory, PhysSize length);
  
  virtual UserProgramMap & CreateMap(anarch::UserMap &);
  
  PhysAddr GetMemory() const;
  PhysSize GetLength() const;
  
private:
  PhysAddr memory;
  PhysSize length;
};

}

}

#endif
