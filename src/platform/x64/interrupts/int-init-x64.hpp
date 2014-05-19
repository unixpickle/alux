#ifndef __PLATFORM_X64_INT_INIT_HPP__
#define __PLATFORM_X64_INT_INIT_HPP__

#include "lapic-x64.hpp"
#include "ioapic-x64.hpp"
#include "pic-x64.hpp"
#include "int-handlers-x64.hpp"

namespace OS {
namespace x64 {

void InitializeInterrupts();
void ConfigureIOAPIC();
void InitializeLAPIC();

}
}

#endif
