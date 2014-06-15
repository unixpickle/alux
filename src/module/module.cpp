#include <module/module.hpp>

namespace OS {

void Module::Load() {
  if (loadStartCount++) {
    return;
  }
  
  DepList deps(GetDependencies(count));
  for (size_t i = 0; i < deps.count; i++) {
    deps[i]->Load();
  }
  
  Initialize();
  
  deps = GetSuperDependencies(count);
  for (size_t i = 0; i < deps.count; i++) {
    deps[i]->Load();
  }
}

bool Module::IsUninitialized() {
  return loadStartCount == 0;
}

DepList Module::GetSuperDependencies() {
  return DepList();
}

}
