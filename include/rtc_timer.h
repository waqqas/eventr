#ifndef RTC_TIMER_H
#define RTC_TIMER_H

#include "eventr.h"
#include "itimer.h"

namespace Eventr {

class rtc_timer : public itimer
{
public:
  rtc_timer(io_handler &io)
    : io(io)
  {
    fd = ::timerfd_create(CLOCK_REALTIME, 0);
  }

  void set_cb(const timer_cb_type &callback) override
  {
    cb = callback;
  }

  void expire_at(const time_t &expiry_epoch)
  {
    itimerspec expiry;
    expiry.it_interval      = {0, 0};
    expiry.it_value.tv_sec  = expiry_epoch;
    expiry.it_value.tv_nsec = 0;

    expire(expiry);
  }

  void expire_in(const time_t &expiry_sec) override
  {
    itimerspec expiry;
    ::clock_gettime(CLOCK_REALTIME, &expiry.it_value);

    expiry.it_interval = {0, 0};
    expiry.it_value.tv_sec += expiry_sec;

    expire(expiry);
  }

  void start() override
  {
    io.add(fd, std::bind(&rtc_timer::on_expiry, this), std::bind(&rtc_timer::on_error, this));
  }

  void stop() override
  {
    io.remove(fd);
  }

  ~rtc_timer()
  {
    ::close(fd);
  }

private:
  void on_expiry()
  {
    cb();
  }

  void on_error()
  {
    io.remove(fd);
    ::close(fd);
  }

  void expire(const itimerspec &expiry)
  {
    if (::timerfd_settime(fd, TFD_TIMER_ABSTIME, &expiry, NULL) == -1)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

private:
  io_handler &  io;
  int           fd;
  timer_cb_type cb;
};
}  // namespace Eventr
#endif
