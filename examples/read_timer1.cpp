// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#include "fd_reader.h"
#include "rtc_timer.h"

#include <iostream>
#include <lyra/lyra.hpp>
#include <string>

using reader_type = Eventr::fd_reader<1024>;
void on_read(reader_type &reader, reader_type::buffer_type buffer, const size_t &size)
{
  static int  count = 5;
  std::string data(buffer.data(), size);
  std::cout << "read : " << data << std::endl;
  if (count-- == 0)
  {
    reader.stop();
  }
}

void on_timer_expired(Eventr::rtc_timer &timer, time_t expiry)
{
  static int count = 5;
  std::cout << "timer expired: " << count << std::endl;
  if (count > 0)
  {
    count--;
    timer.expire_in(expiry);
  }
  else
  {
    timer.stop();
  }
}

int main(int argc, char *argv[])
{
  Eventr::io_handler io(10);
  Eventr::rtc_timer  timer(io);
  reader_type        reader(io, STDIN_FILENO);

  time_t expiry = 1;

  auto cli =
      lyra::cli_parser() | lyra::opt(expiry, "expiry")["-e"]["--expiry"]("Expiry time in seconds?");

  auto result = cli.parse({argc, argv});
  if (!result)
  {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    exit(1);
  }

  timer.set_cb(std::bind(on_timer_expired, std::ref(timer), expiry));
  timer.expire_in(expiry);
  timer.start();

  reader.set_cb(std::bind(on_read, std::ref(reader), std::placeholders::_1, std::placeholders::_2));
  reader.start();

  io.run();

  return 0;
}