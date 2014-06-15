#include <module/module.hpp>
#include <panic>

namespace OS {

Module::Module() : loadStartCount(0) {}

Module::~Module() {
  Panic("A module cannot be destroyed!");
}

void Module::Load() {
  if (loadStartCount++) {
    return;
  }
  
  DepList deps(GetDependencies());
  for (int i = 0; i < deps.count; i++) {
    deps[i]->Load();
  }
  
  Initialize();
  
  deps = GetSuperDependencies();
  for (int i = 0; i < deps.count; i++) {
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
