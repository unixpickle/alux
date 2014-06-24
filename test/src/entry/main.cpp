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
  fork((void *)ForkedCall);
}

void ForkedCall() {
  std::puts("this is coming from a subprocess\n");
}
