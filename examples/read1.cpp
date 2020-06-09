// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#include "fd_reader.h"

#include <iostream>
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

int main(void)
{
  Eventr::io_handler io(10);
  reader_type        reader(io, STDIN_FILENO);

  reader.set_cb(std::bind(on_read, std::ref(reader), std::placeholders::_1, std::placeholders::_2));
  reader.start();

  io.run();

  return 0;
}