#ifndef TIMER_H
#define TIMER_H

#include <sys/timerfd.h>

namespace Eventr {
class timer
{
public:
  timer()
  {
    fd = ::timerfd_create();
  }

  ~timer()
  {
    ::close(fd);
  }

private:
  int fd;
};
}  // namespace Eventr
#endif
