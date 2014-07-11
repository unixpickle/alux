#include <memory/malloc-link.hpp>

namespace OS {

MallocLink * MallocLink::Wrap(uint8_t * start, size_t length) {
  size_t used = sizeof(MallocLink);
  MallocLink * link = (MallocLink *)start;
  link->malloc = ANAlloc::Malloc::WrapRegion<TreeType>(start, length, 6, used);
  link->next = NULL;
  return link;
}

}
