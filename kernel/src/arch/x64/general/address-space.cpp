#include <arch/general/address-space.hpp>

namespace OS {

int AddressSpace::GetPageSizeCount() {
  return 2;
}

size_t AddressSpace::GetPageSize(int index) {
  if (index) return 0x200000;
  return 0x1000;
}

size_t AddressSpace::GetPageAlignment(int index) {
  return GetPageSize(index);
}

}
