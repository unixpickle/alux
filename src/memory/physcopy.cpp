#include <memory/physcopy.hpp>
#include <memory/easy-map.hpp>
#include <cstring>

namespace OS {

void PhysCopy(void * dest, PhysAddr source, size_t size) {
  EasyMap map(source, size, false);
  memcpy(dest, (uint8_t *)map.GetStart(), size);
}

}
