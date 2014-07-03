#include <arch/general/syscall.hpp>
#include <iostream>
#include <cstddef>

namespace Test {

ConsoleStream cout;
ErrorStream cerr;
const char * endl = "\n";

void Puts(const char * buff, Color color, bool bright) {
  Print(buff, color, bright);
  Print("\n", color, bright);
}

void Print(const char * buff, Color color, bool bright) {
  ArgList list;
  list.PushVirtAddr((VirtAddr)buff);
  list.PushUInt64((uint64_t)color);
  list.PushBool(bright);
  Syscall::Run(Syscall::Print, list);
}

void ConsoleStream::PrintBuffer(const char * buf) {
  Print(buf, ColorLightGray, true);
}

void ErrorStream::PrintBuffer(const char * buf) {
  Print(buf, ColorRed, true);
}

OutStream & operator<<(OutStream & stream, const char * string) {
  stream.PrintBuffer(string);
  return stream;
}

OutStream & operator<<(OutStream & stream, uint8_t number) {
  return stream << (uint64_t)number;
}

OutStream & operator<<(OutStream & stream, uint16_t number) {
  return stream << (uint64_t)number;
}

OutStream & operator<<(OutStream & stream, uint32_t number) {
  return stream << (uint64_t)number;
}

OutStream & operator<<(OutStream & stream, uint64_t number) {
  const char * chars = "0123456789abcdef";
  unsigned char _buf[32] = {0};
  _buf[0] = '0';
  _buf[1] = 'x';
  unsigned char * buf = _buf + 2;
  unsigned char len = 2, i;
  do {
    unsigned char nextDig = (unsigned char)(number & 0xf);
    buf[len++] = chars[nextDig];
    number >>= 4;
  } while (number > 0);
  for (i = 0; i < len / 2; i++) {
    unsigned char a = buf[len - i - 1];
    buf[len - i - 1] = buf[i];
    buf[i] = a;
  }
  
  stream << (const char *)_buf;
  
  return stream;
}

}
