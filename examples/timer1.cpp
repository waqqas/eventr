#include "config.h"
#include "timer.h"

#include <iostream>
#include <lyra/lyra.hpp>

void on_timer_expired(Eventr::rtc_timer *timer)
{
  std::cout << "timer expired" << std::endl;
  timer->stop();
}

int main(int argc, char *argv[])
{
  time_t expiry = 5;

  auto cli =
      lyra::cli_parser() | lyra::opt(expiry, "expiry")["-e"]["--expiry"]("Expiry time in seconds?");

  auto result = cli.parse({argc, argv});
  if (!result)
  {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    exit(1);
  }

  Eventr::io_handler io(10);
  Eventr::rtc_timer  timer(io);

  timer.set_cb(std::bind(on_timer_expired, &timer));
  timer.expire_in(expiry);

  // timespec now;
  // ::clock_gettime(CLOCK_REALTIME, &now);
  // timer.expire_at(now.tv_sec + 20);

  timer.start();

  io.run();

  return 0;
}