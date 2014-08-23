#ifndef __ALUX_EXECUTABLE_HPP__
#define __ALUX_EXECUTABLE_HPP__

#include <anarch/api/user-map>
#include "executable-map.hpp"

namespace Alux {

class Executable {
public:
  virtual ExecutableMap & GenerateMap(anarch::UserMap &) = 0;
};

}

#endif
