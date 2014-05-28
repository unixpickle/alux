#include <memory/easy-map.hpp>
#include <arch/general/address-space.hpp>

namespace OS {

EasyMap::EasyMap(PhysAddr _start, size_t _size, bool exec) {
  AddressSpace & space = AddressSpace::GetGlobal();
  
  // this little algorithm will find the page size we should use
  pageAlign = space.GetPageAlignment(0);
  pageSize = space.GetPageSize(0);
  for (int i = 1; i < space.GetPageSizeCount(); i++) {
    size_t alignment = space.GetPageAlignment(i);
    size_t size = space.GetPageSize(i);
    size_t reqSize = _size;
    if (_start % alignment) {
      reqSize += alignment - (_start % alignment);
    }
    if (reqSize < size) break;
    pageSize = size;
    pageAlign = alignment;
  }
  
  PhysAddr aligned = _start;
  size_t endSize = _size;
  if (aligned % pageAlign) {
    endSize += pageAlign - (aligned % pageAlign);
    aligned -= pageAlign - (aligned % pageAlign);
  }
  
  pageCount = endSize / pageSize + (endSize % pageSize ? 1 : 0);
  
  mapStart = space.Map(aligned, pageSize, pageCount, exec);
  start = mapStart + (_start - aligned);
}

EasyMap::~EasyMap() {
  AddressSpace & space = AddressSpace::GetGlobal();
  space.Unmap(mapStart, pageSize, pageCount);
}

VirtAddr EasyMap::GetStart() const {
  return start;
}

}
