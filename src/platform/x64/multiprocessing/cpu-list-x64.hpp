#ifndef __PLATFORM_X64_CPU_LIST_X64_HPP__
#define __PLATFORM_X64_CPU_LIST_X64_HPP__

#include <cassert>
#include <new>
#include "cpu-x64.hpp"
#include "../gdt-x64.hpp"

namespace OS {

namespace x64 {

class CPUList {
private:
  int capacity;
  int count;
  uint8_t * buffer;

public:
  static void Initialize(int capacity);
  static CPUList & GetGlobal();
  
  CPUList() {
    Panic("This is to make the compiler happy!");
  }
  
  CPUList(int _capacity);
  
  int GetCount();
  int ConstructEntry(uint32_t apicId);
  
  CPU & operator[](int idx);
  CPU & GetCurrent();
};

}

}

#endif
