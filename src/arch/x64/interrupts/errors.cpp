#include <arch/x64/interrupts/errors.hpp>
#include <arch/x64/interrupts/idt.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <panic>
#include <new>

namespace OS {

namespace x64 {

static InterruptErrors gErrors;

void InterruptErrors::InitGlobal() {
  new(&gErrors) InterruptErrors();
}

InterruptErrors & InterruptErrors::GetGlobal() {
  return gErrors;
}

void InterruptErrors::Initialize() {
  IRT::GetGlobal()[0x2] = &InterruptErrors::HandleNMI;
  IRT::GetGlobal()[0x12] = &InterruptErrors::HandleMCE;
  IDT::GetGlobal().SetIST(0x2, 1);
  IDT::GetGlobal().SetIST(0x12, 1);
}

DepList InterruptErrors::GetDependencies() {
  return DepList(&IDT::GetGlobal(), &IRT::GetGlobal());
}

void InterruptErrors::HandleNMI() {
  Panic("received unexpected non-maskable interrupt");
}

void InterruptErrors::HandleMCE() {
  Panic("received unexpected machine check");
}

}

}
