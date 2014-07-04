#include <ctime>
#include <cunistd>
#include <iostream>

namespace Test {

void TestBasicTimeout() {
  uint64_t now = utime();
  cout << "boot time is " << now << ", waiting 0x100000 usecs..." << endl;
  usleep(0x100000 + now);
  uint64_t diff = utime() - now;
  cout << "difference is " << diff << endl;
}

}