#ifndef __SCHEDULER_MESSAGE_HPP__
#define __SCHEDULER_MESSAGE_HPP__

namespace OS {

class Message {
public:
  Message * next;
  char data[0x40];
};

}

#endif
