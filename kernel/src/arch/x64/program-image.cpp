#include "program-image.hpp"
#include <ansa/cstring>

namespace Alux {

namespace x64 {

ProgramImage::ProgramImage()
  : kernelEnd(*(uint32_t *)0x100020 + 0x100000),
    programSize(*(uint32_t *)0x100024) {
  programStart = (PhysAddr)kernelEnd;
  if (programStart % 0x200000) {
    programStart += 0x200000 - (programStart % 0x200000);
    ansa::Memmove((void *)programStart, (void *)kernelEnd,
                  (size_t)programSize);
  }
  if (programSize % 0x200000) {
    programSize += 0x200000 - (programSize % 0x200000);
  }
}

}

}
