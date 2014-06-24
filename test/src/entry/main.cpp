#include <iostream>
#include <cunistd>

void Main();

extern "C" {

void main() {
  Main();
}

void _main() {
  Main();
}

}

void ForkedCall();

void Main() {
  std::puts("this is the parent process", std::ColorLightGray, false);
  if (getpid() != 1) {
    std::puts("weird process ID found (expected 1)", std::ColorRed, true);
  }
  fork((void *)ForkedCall);
}

void ForkedCall() {
  if (getpid() != 2) {
    std::puts("weird process ID found (expected 2)", std::ColorRed, true);
  }
  std::puts("this is coming from a subprocess\n");
}
