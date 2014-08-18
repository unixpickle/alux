#ifndef __ALUX_APP_HPP__
#define __ALUX_APP_HPP__

#include "app-code-map.hpp"

namespace OS {

class App {
public:
  virtual AppCodeMap & CreateMap(anarch::UserMap &) = 0;
};

}

#endif
