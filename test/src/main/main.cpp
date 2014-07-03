#include <iostream>
#include <cunistd>
#include <ctime>
#include <arch/general/tests.hpp>
#include <new>

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
  
  cout << "current time is " << utime() << endl;
  
  cout << "counting to five..." << endl;
  for (uint32_t i = 0; i < 5; i++) {
    usleep(utime() + 1000000);
    cout << " " << i + 1;
  }
  cout << endl;
}

extern "C" {

void main() {
  Main();
}

void _main() {
  Main();
}

}
