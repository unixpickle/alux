#include "port.hpp"
#include "errors.hpp"
#include "../tasks/hold-scope.hpp"

namespace Alux {

anarch::SyscallRet CreatePortSyscall() {
  HoldScope scope;
  ThreadPort & p = ThreadPort::New(scope.GetThread());
  if (!p.AddToThread()) {
    p.Dealloc(false);
    return anarch::SyscallRet::Error(SyscallErrorPortsListFull);
  }
  return anarch::SyscallRet::Integer32((uint32_t)p.GetIdentifier());
}

anarch::SyscallRet DestroyPortSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  anidmap::Identifier ident = (anidmap::Identifier)args.PopUInt32();
  ThreadPort * port = scope.GetThread().GetPortList().Find(ident);
  if (!port) {
    return anarch::SyscallRet::Error(SyscallErrorNoPort);
  }
  scope.GetThread().GetPortList().Remove(*port);
  port->Sever();
  port->Dealloc(true);
  return anarch::SyscallRet::Empty();
}

}
