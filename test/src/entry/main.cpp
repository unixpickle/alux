void Main();

extern "C" {

void main() {
  Main();
}

void _main() {
  Main();
}

}

void Main() {
  __asm__("syscall");
}
