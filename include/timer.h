#ifndef TIMER_H
#define TIMER_H

#include "eventr.h"

#include <functional>
#include <iostream>
#include <sys/timerfd.h>

namespace Eventr {
class rtc_timer
{
public:
  using timer_cb = std::function<void(void)>;

  rtc_timer(io_handler &io)
    : io(io)
  {
    fd = ::timerfd_create(CLOCK_REALTIME, 0);
  }

  void set_cb(const timer_cb &callback)
  {
    cb = callback;
  }

  void expire_at(const itimerspec &new_value)
  {
    if (::timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  void expire_in(const time_t &expiry_sec)
  {
    timespec now;

    ::clock_gettime(CLOCK_REALTIME, &now);

    expiry.it_interval      = {0, 0};
    expiry.it_value.tv_sec  = now.tv_sec + expiry_sec;
    expiry.it_value.tv_nsec = now.tv_nsec;
  }

  void start()
  {
    std::cout << "expiry: " << expiry.it_value.tv_sec << ":" << expiry.it_value.tv_nsec
              << std::endl;
    if (::timerfd_settime(fd, TFD_TIMER_ABSTIME, &expiry, NULL) == -1)
    {
      throw std::runtime_error(::strerror(errno));
    }
    io.add(fd, std::bind(&rtc_timer::on_expiry, this));
  }

  void stop()
  {
    io.remove(fd);
  }

  void on_expiry()
  {
    cb();
  }

  ~rtc_timer()
  {
    stop();
    ::close(fd);
  }

private:
  int         fd;
  io_handler &io;
  timer_cb    cb;
  itimerspec  expiry;
};
}  // namespace Eventr
#endif
