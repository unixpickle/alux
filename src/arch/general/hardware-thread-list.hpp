#ifndef __GENERAL_HARDWARE_THREAD_LIST_HPP__
#define __GENERAL_HARDWARE_THREAD_LIST_HPP__

#include <arch/general/hardware-thread.hpp>

namespace OS {

class HardwareThreadList {
public:
  static HardwareThreadList & GetGlobal();
  
  virtual int GetCount() = 0;
  virtual HardwareThread & operator[](int index) = 0;
  virtual int IndexOf(HardwareThread & th) = 0;
};

}

#endif
