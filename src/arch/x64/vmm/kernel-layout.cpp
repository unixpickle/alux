#include <arch/x64/vmm/kernel-layout.hpp>

namespace OS {

namespace x64 {

size_t KernelSize() {
  // TODO: use some sort of symbol here or *something*
  return 0x1000000L;
}

}

}
