#ifndef __ALUX_APP_CODE_MAP_HPP__
#define __ALUX_APP_CODE_MAP_HPP__

#include <anarch/types>
#include <anarch/api/user-map>

namespace OS {

class App;

class AppCodeMap {
public:
  virtual void * GetEntryPoint() = 0; // @ambicritical
  virtual App & GetApp() = 0; // @ambicritical
  
  virtual bool HandlePageFault(VirtAddr addr, bool write) = 0; // @noncritical
  virtual void Delete() = 0; // @noncritical
  
  anarch::UserMap & GetMemoryMap() const {
    return memoryMap;
  }
  
protected:
  AppCodeMap(anarch::UserMap & map) : memoryMap(map) {}
  virtual ~AppCodeMap() {}
  
  anarch::UserMap & memoryMap;
};

}

#endif
