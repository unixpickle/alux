#include "user-port.hpp"

namespace Alux {

UserPort * UserPort::New(Thread & th) {
  return new UserPort(th);
}

void UserPort::Delete() {
  delete this;
}

void UserPort::Send(const Data &) {
}

UserPort::UserPort(Thread & th)
  : waitingLink(*this), hashMapLink(*this), thread(th) {
}

}
