// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#include "udp_socket.h"
#include "io_handler.h"

#include <iostream>
#include <lyra/lyra.hpp>

using receiver_type = Eventr::udp_socket<2048>;

void on_receive(receiver_type &receiver, const receiver_type::buffer_type buffer,
                const size_t &size, const sockaddr_in &remote_addr)
{
  static int  count = 5;
  std::string data(buffer.data(), size);
  std::cout << "read : " << data << std::endl;

  try
  {
    receiver.send(buffer.data(), size, remote_addr);
  }
  catch (std::exception &e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }

  if (count-- == 0)
  {
    receiver.stop();
  }
}

int main(int argc, char *argv[])
{
  Eventr::io_handler io(10);
  receiver_type      receiver(io);

  uint32_t    port = 5000;
  std::string ip   = "0.0.0.0";
  auto        cli  = lyra::cli_parser() | lyra::opt(port, "port")["-p"]["--port"]("Server port") |
             lyra::opt(port, "ip")["-i"]["--ip"]("Server IP");

  auto result = cli.parse({argc, argv});
  if (!result)
  {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    exit(1);
  }

  try
  {
    receiver.set_cb(std::bind(on_receive, std::ref(receiver), std::placeholders::_1,
                              std::placeholders::_2, std::placeholders::_3));
    receiver.bind(ip, port);
    receiver.start();

    io.run();
  }
  catch (std::exception &e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }

  return 0;
}