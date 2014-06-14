#include <module/module.hpp>

namespace OS {

void Module::Load() {
  if (loadStartCount++) {
    return;
  }
  
  size_t count;
  Module ** deps = GetDependencies(count);
  for (size_t i = 0; i < count; i++) {
    deps[i]->Load();
  }
  
  Initialize();
  
  size_t count;
  Module ** deps = GetSuperDependencies(count);
  for (size_t i = 0; i < count; i++) {
    deps[i]->Load();
  }
}

bool Module::IsUninitialized() {
  return loadStartCount == 0;
}

Module ** Module::GetSuperDependencies(size_t & count) {
  count = 0;
  return NULL;
}

}
