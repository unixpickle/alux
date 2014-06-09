#ifndef __GENERAL_CPU_LIST_HPP__
#define __GENERAL_CPU_LIST_HPP__

#include <arch/general/cpu.hpp>

namespace OS {

class CPUList {
public:
  static CPUList & GetGlobal();
  
  virtual int GetCount() = 0;
  virtual CPU & operator[](int index) = 0;
};

}

#endif
