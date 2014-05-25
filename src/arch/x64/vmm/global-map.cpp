#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/vmm/map-setup.hpp>
#include <cstddef>
#include <new>

namespace OS {

namespace x64 {

static GlobalMap instance;

void GlobalMap::Initialize(PageAllocator * anAlloc) {
  new(&instance) GlobalMap();
  instance.allocator = anAlloc;
  instance.Setup();
}

GlobalMap & GlobalMap::GetGlobal() {
  return instance;
}

GlobalMap::GlobalMap() : table(0), allocator(NULL), pdpt(0) {
}

PhysAddr GlobalMap::GetPDPT() {
  return pdpt;
}
  
void GlobalMap::Setup() {
  MapSetup setup(*allocator);
  setup.Map();
  
  table.SetPML4(setup.GetPML4());
  pdpt = setup.GetPDPT();
}

}

}
