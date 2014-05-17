#ifndef __PLATFORM_X64_IDT_X64_H__
#define __PLATFORM_X64_IDT_X64_H__

#include "idt-structures-x64.hpp"
#include <cassert>

namespace OS {

namespace x64 {

class InterruptTable {
private:
  IdtEntry entries[0x100] OS_ALIGNED(8);
  IntHandler handlers[0x100];
  IdtPointer idtPtr OS_ALIGNED(8);
  
public:
  void SetHandler(int idx, void * fn, uint8_t flags);
  void Load();
};

}

}

#endif

