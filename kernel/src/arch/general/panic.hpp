#ifndef __GENERAL_PANIC_HPP__
#define __GENERAL_PANIC_HPP__

#include <macros>

namespace OS {

void Panic(const char * msg) OS_NORETURN;

}

#endif
