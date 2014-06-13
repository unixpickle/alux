#ifndef __X64_INTERRUPT_ERRORS_HPP__
#define __X64_INTERRUPT_ERRORS_HPP__

namespace OS {

namespace x64 {

void HandleNMIReceived();
void HandleMCEReceived();

}

}

#endif
