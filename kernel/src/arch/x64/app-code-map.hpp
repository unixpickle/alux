#ifndef __ALUX_X64_APP_CODE_MAP_HPP__
#define __ALUX_X64_APP_CODE_MAP_HPP__

#include "../all/app-code-map.hpp"
#include <anarch/lock>

namespace OS {

namespace x64 {

class App;

class AppCodeMap : public OS::AppCodeMap {
public:
  static const VirtAddr StartAddr = 0x8000000000UL;
  static const PhysSize SectorSize = 0x200000;
  
  static AppCodeMap & New(anarch::UserMap & map, App & prog);
  
  virtual void * GetEntryPoint();
  virtual OS::App & GetApp();
  
  virtual bool HandlePageFault(VirtAddr addr, bool write);
  virtual void Delete();
  
private:
  /**
   * A 2MB sector.
   */
  struct Sector {
    bool readable = false;
    bool writable = false;
    PhysAddr * writables = NULL;
    VirtAddr startAddr;
    PhysAddr readOnlyStart;
  };
  
  AppCodeMap(anarch::UserMap & map, App & prog);
  virtual ~AppCodeMap();
  
  void HandleReadFault(Sector & sector);
  void HandleWriteFault(Sector & sector, VirtAddr pageAddr);
  void HandleFirstWriteFault(Sector & sector, VirtAddr pageAddr);
  
  App & app;
  
  anarch::NoncriticalLock lock;
  Sector * sectors;
  int sectorCount;
};

}

}

#endif
