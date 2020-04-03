#include "config.h"
#include "timer.h"

#include <iostream>
#include <lyra/lyra.hpp>

void on_timer_expired(void)
{
  std::cout << "timer expired" << std::endl;
}

int main(void)
{
  Eventr::io_handler io(10, 10);
  Eventr::rtc_timer  timer(io);

  time_t sec = 10;

  timer.set_cb(on_timer_expired);
  // timer.expire_at();
  timer.expire_in(sec);
  timer.start();

  io.run();

  return 0;
}