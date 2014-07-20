#include "main.hpp"
#include <anarch/x64/multiboot-region-list>
#include <anarch/x64/init>
#include <anarch/api/global-map>
#include <anarch/api/panic>
#include <anarch/stream>
#include <ansa/macros>
#include <ansa/cstring>

extern "C" {

void AluxMainX64(void * mbootPtr) {
  anarch::x64::InitializeSingletons();
  
  anarch::StreamModule::GetGlobal().Load();
  anarch::cout << "AluxMainX64(" << (uint64_t)mbootPtr << ")"
    << anarch::endl;
  
  __asm__ __volatile__("cli\nhlt"); // TODO: delete this
  
  // this stack is going to be preserved throughout the runtime of the OS, so
  // it is acceptable to store the boot info here
  
  anarch::x64::MultibootRegionList regions(mbootPtr);
  
  // TODO: here, calculate the length of the kernel by loading the program
  anarch::x64::BootInfo bootInfo(regions, 0x300000);
  
  anarch::x64::SetBootInfo(&bootInfo);
  
  // load the global map
  anarch::GlobalMap::GetGlobal().Load();
  
  anarch::cout << "finished loading GlobalMap" << anarch::endl;
}

}
