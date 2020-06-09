// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#ifndef EVENTR_ITIMER_H
#define EVENTR_ITIMER_H

#include <functional>
#include <sys/timerfd.h>

namespace Eventr {
class itimer
{
public:
  using timer_cb_type = std::function<void(void)>;

  virtual void start()                               = 0;
  virtual void stop()                                = 0;
  virtual void expire_in(const time_t &expiry_sec)   = 0;
  virtual void set_cb(const timer_cb_type &callback) = 0;
  virtual ~itimer()
  {}
};
}  // namespace Eventr

#endif