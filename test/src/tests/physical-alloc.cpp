#include <memory/wired.hpp>
#include <iostream>

namespace Test {

void TestPhysicalAllocation() {
  cout << "physical used=" << WiredMemory::PhysicalUsed() << endl;
  cout << "physical free=" << WiredMemory::PhysicalAvailable()
    << endl;
  cout << "physical total=" << WiredMemory::PhysicalTotal() << endl;
  
  cout << "there are " << (uint64_t)WiredMemory::GetPageSizeCount()
    << " page sizes:" << endl;
  for (int i = 0; i < WiredMemory::GetPageSizeCount(); i++) {
    cout << (uint64_t)i << " - size=" << WiredMemory::GetPageSize(i)
      << " alignment=" << WiredMemory::GetPageAlignment(i) << endl;
  }
  
  cout << "testing page allocation..." << endl;
  for (int i = 0; i < WiredMemory::GetPageSizeCount(); i++) {
    cout << (uint64_t)i << " - ";
    PhysAddr addr;
    bool result = WiredMemory::Allocate(addr, WiredMemory::GetPageSize(i),
                                        WiredMemory::GetPageAlignment(i));
    if (!result) {
      cerr << "failed" << endl;
    } else {
      cout << "address=" << (uint64_t)addr << " used="
        << WiredMemory::PhysicalUsed();
      WiredMemory::Free(addr);
      cout << " after free=" << WiredMemory::PhysicalUsed() << endl;
    }
  }
}

}