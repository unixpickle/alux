#ifndef __ALUX_X64_USER_PROGRAM_HPP__
#define __ALUX_X64_USER_PROGRAM_HPP__

#include "../all/user-program.hpp"
#include "user-program-map.hpp"

namespace OS {

namespace x64 {

class UserProgram : public OS::UserProgram {
public:
  virtual UserProgramMap & CreateMap();
};

}

}

#endif
