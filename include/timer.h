#ifndef TIMER_H
#define TIMER_H

#include "event.h"

#include <sys/timerfd.h>

namespace Eventr {
class timer : public event
{
  timer()
  {
    fd = ::timerfd_create();
  }
};
}  // namespace Eventr
#endif
