#ifndef __ALUX_X64_LAYOUT_HPP__
#define __ALUX_X64_LAYOUT_HPP__

#include <anarch/types>

namespace OS {

namespace x64 {

struct Layout {
  Layout(void * loadAddr);
  void AlignProgram();
  
  void * kernelStart;
  uint32_t kernelSize;
  
  void * programStart;
  uint32_t programSize;
};

}

}

#endif
