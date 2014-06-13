#include <panic>

namespace OS {

namespace x64 {

void HandleNMIReceived() {
  Panic("OS::x64::HandleNMIReceived() - fatal error");
}

void HandleMCEReceived() {
  Panic("OS::x64::HandleNCEReceived() - fatal error");
}

}

}
