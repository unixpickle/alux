#include <platform/failure.hpp>
#include <cstddef>
#include <iostream>
#include "interrupts/irt-x64.hpp"
#include "interrupts/int-vectors-x64.hpp"
#include "interrupts/lapic-x64.hpp"
#include "multiprocessing/cpu-list-x64.hpp"

namespace OS {
namespace x64 {

void InitializeFailure();
void FailureIntHandler();

}
}
