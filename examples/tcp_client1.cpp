#include "tcp_comm_socket.h"

#include <iostream>
#include <lyra/lyra.hpp>
#include <string>

using socket_type = Eventr::tcp_comm_socket<2048>;

void on_connect(socket_type &client)
{
  std::cout << "client connected" << std::endl;
  client.send("hello", sizeof("hello"));
}

void on_receive(socket_type &client, const socket_type::buffer_type buffer, const size_t &size)
{
  static int  count = 5;
  std::string data(buffer.data(), size);
  std::cout << "read : " << data << std::endl;

  //   try
  //   {
  //     server.send(buffer.data(), size);
  //   }
  //   catch (std::exception &e)
  //   {
  //     std::cout << "exception: " << e.what() << std::endl;
  //   }

  if (count-- == 0)
  {
    client.stop();
  }
}

int main(int argc, char *argv[])
{
  uint32_t    server_port = 5000;
  std::string server_ip   = "127.0.0.1";

  auto cli = lyra::cli_parser() |
             lyra::opt(server_port, "port")["-p"]["--port"]("TCP port to connect to") |
             lyra::opt(server_ip, "ip")["-i"]["--ip"]("TCP port to connct to");
  ;

  auto result = cli.parse({argc, argv});
  if (!result)
  {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    exit(1);
  }

  Eventr::io_handler io(10);
  socket_type        client(io);

  client.set_on_connect(std::bind(on_connect, std::ref(client)));
  client.set_on_receive(
      std::bind(on_receive, std::ref(client), std::placeholders::_1, std::placeholders::_2));

  client.start();

  client.connect(server_ip, server_port);

  io.run();

  return 0;
}