#ifndef __PLATFORM_X64_IDT_X64_H__
#define __PLATFORM_X64_IDT_X64_H__

#include "idt-structures-x64.hpp"
#include <cassert>
#include <new>

namespace OS {

namespace x64 {

class IDT {
private:
  IdtEntry entries[0x100] OS_ALIGNED(8);
  IntHandler handlers[0x100];
  IdtPointer idtPtr OS_ALIGNED(8);
  
public:
  static void Initialize();
  static IDT & GetGlobal();
  
  void SetHandler(int idx, void * fn, uint8_t flags);
  void Load();
};

}

}

#endif

