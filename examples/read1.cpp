#include "fd_reader.h"

#include <iostream>
#include <lyra/lyra.hpp>

void on_read(Eventr::fd_reader &reader)
{
  std::cout << "read : " << std::endl;
  reader.stop();
}

int main(void)
{
  Eventr::io_handler  io(10);
  Eventr::fd_reader reader(io, STDIN_FILENO);

  reader.set_cb(std::bind(on_read, std::ref(reader)));
  reader.start();

  io.run();

  return 0;
}