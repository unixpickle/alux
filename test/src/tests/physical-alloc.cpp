#include <memory/wired.hpp>
#include <iostream>
#include <ctime>

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
  
  uint64_t start = utime();
  for (int i = 0; i < 0x10000; i++) {
    PhysAddr addr;
    bool result = WiredMemory::Allocate(addr, WiredMemory::GetPageSize(0),
                                        WiredMemory::GetPageAlignment(0));
    if (!result) {
      cerr << "failed to allocate for timed test" << endl;
    } else {
      WiredMemory::Free(addr);
    }
  }
  cout << "0x10000 page allocations took " << utime() - start << " micros"
    << endl;
}

}