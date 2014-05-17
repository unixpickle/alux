#include "size-config-x64.hpp"

namespace OS {

namespace x64 {
  
size_t KernelDataSize() {
  return 0x1000000; // TODO: use more information here
}
  
void * ScratchPTStart() {
  return (void *)(KernelDataSize() - (0x1000 * ScratchPTCount));
}

}

}
