#ifndef __ALUX_USER_PROGRAM_HPP__
#define __ALUX_USER_PROGRAM_HPP__

#include "user-program-map.hpp"

namespace OS {

class UserProgram {
public:
  virtual UserProgramMap & CreateMap(anarch::UserMap &) = 0;
};

}

#endif
