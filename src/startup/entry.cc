#include "entry.hpp"

namespace OS {

void EntryPoint() {
  size_t amountFree = PhysicalAvailable();
  size_t amountTaken = PhysicalUsed();
  cout << "OS::EntryPoint() - using " << amountTaken
    << " bytes out of " << amountTaken + amountFree << endl;
  
  cout << "Running OS::InitializeMalloc()...";
  InitializeMalloc();
  cout << " [OK]" << endl;

  InitializeScheduler();
  
  Panic("nothing to do now");
}

}
