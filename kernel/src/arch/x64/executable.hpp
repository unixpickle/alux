#ifndef __ALUX_X64_EXECUTABLE_HPP__
#define __ALUX_X64_EXECUTABLE_HPP__

#include "../all/executable.hpp"
#include "executable-map.hpp"

namespace Alux {

namespace x64 {

class Executable : public Alux::Executable {
public:
  /**
   * Create an executable with a 2MB aligned region of memory. The length of
   * the memory and the address of the memory must BOTH be 2MB aligned.
   */
  Executable(PhysAddr memory, size_t length);
  
  /**
   * Create a new executable map that will copy code from this region.
   */
  virtual ExecutableMap & GenerateMap(anarch::UserMap &);
  
  inline PhysAddr GetMemory() const {
    return memory;
  }
  
  inline const char * GetReadableMemory() const {
    return (const char *)memory;
  }
  
  inline size_t GetLength() const {
    return length;
  }
  
private:
  PhysAddr memory;
  size_t length;
};

}

}

#endif
