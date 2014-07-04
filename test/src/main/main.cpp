#include <iostream>
#include <cunistd>
#include <ctime>
#include <new>
#include <arch/general/tests.hpp>
#include <tests/basic-timeout.hpp>
#include <tests/physical-alloc.hpp>

using namespace Test;

void Main() {
  new(&cout) ConsoleStream();
  new(&cerr) ErrorStream();
  
  cout << "testing basic threading...";
  if (!BasicThreadTest()) {
    cerr << "[fail]" << endl;
    return;
  }
  cout << "[OK]" << endl;
  
  cout << "testing fork+thread...";
  if (!ForkAndThreadTest()) {
    cerr << "[fail]" << endl;
    return;
  }
  cout << "[OK]" << endl;
  
  TestBasicTimeout();
  TestPhysicalAllocation();
  
  cout << "tests complete!" << endl;
}

extern "C" {

void main() {
  Main();
}

void _main() {
  Main();
}

}
