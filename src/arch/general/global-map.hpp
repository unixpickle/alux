#ifndef __GENERAL_GLOBAL_MAP_HPP__
#define __GENERAL_GLOBAL_MAP_HPP__

#include <arch/general/address-space.hpp>
#include <module/module.hpp>

namespace OS {

class GlobalMap : public AddressSpace, public Module {
public:
  static GlobalMap & GetGlobal();
};

}

#endif
