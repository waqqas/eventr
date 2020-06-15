// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#include "io_handler.h"
#include "udp_socket.h"

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

  uint32_t    port               = 5000;
  std::string multicast_group_ip = "224.0.0.1";
  std::string local_ip           = "0.0.0.0";
  auto        cli =
      lyra::cli_parser() | lyra::opt(port, "port")["-p"]["--port"]("Server port") |
      lyra::opt(local_ip, "local-ip")["-i"]["--local-ip"]("Local IP address") |
      lyra::opt(multicast_group_ip, "mulicast-ip")["-m"]["--multicast-ip"]("Multicast IP address");

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
    receiver.bind(local_ip, port);
    
    receiver.set_option(SO_REUSEADDR, 1U);

    ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(multicast_group_ip.c_str());
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    receiver.set_option(IP_ADD_MEMBERSHIP, mreq, IPPROTO_IP);

    receiver.start();

    io.run();
  }
  catch (std::exception &e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }

  return 0;
}