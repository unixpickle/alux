#include <arch/x64/vmm/invlpg.hpp>

namespace OS {

namespace x64 {

void DistributeKernelInvlpg(VirtAddr start, size_t size) {
  Invlpg(start, size);
  // TODO: here, loop through each CPU and send it an IPI; then, wait until
  // all the CPUs are done.
}

}

}