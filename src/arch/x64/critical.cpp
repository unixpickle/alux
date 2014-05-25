namespace OS {

static bool ignoreCriticality = true;

void SetIgnoreCriticality(bool flag) {
  ignoreCriticality = flag;
}

bool IgnoreCriticality() {
  return ignoreCriticality;
}

bool GetCritical() {
  unsigned long value;
  __asm__("pushfq\n"
          "pop %0" : "=r" (value));
  return (value & (1 << 9)) == 0;
}

void SetCritical(bool flag) {
  if (flag) {
    __asm__("cli");
  } else {
    __asm__("sti");
  }
}

}
