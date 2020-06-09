// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#include "rtc_timer.h"
#include "io_handler.h"

#include <iostream>
#include <lyra/lyra.hpp>

void on_timer_expired(Eventr::itimer *timer, time_t expiry)
{
  static int count = 5;
  std::cout << "timer expired: " << count << std::endl;
  if (count > 0)
  {
    count--;
    timer->expire_in(expiry);
  }
  else
  {
    timer->stop();
  }
}

int main(int argc, char *argv[])
{
  time_t expiry = 1;

  auto cli =
      lyra::cli_parser() | lyra::opt(expiry, "expiry")["-e"]["--expiry"]("Expiry time in seconds?");

  auto result = cli.parse({argc, argv});
  if (!result)
  {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    exit(1);
  }

  Eventr::io_handler io(10);
  Eventr::itimer *   timer = new Eventr::rtc_timer(io);

  timer->set_cb(std::bind(on_timer_expired, timer, expiry));
  timer->expire_in(expiry);

  // timespec now;
  // ::clock_gettime(CLOCK_REALTIME, &now);
  // timer.expire_at(now.tv_sec + 20);

  timer->start();

  io.run();

  delete (timer);

  return 0;
}