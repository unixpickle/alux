void Main();

extern "C" {

void main() {
  Main();
}

}

void Main() {
  __asm__("syscall");
}
