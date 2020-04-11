#include "tcp_comm_socket.h"
#include "tcp_server_socket.h"

#include <iostream>
#include <lyra/lyra.hpp>

#define UNUSED(x) (void)(x)

using sever_socket_type = Eventr::tcp_server_socket<2048>;
using comm_socket_type  = typename sever_socket_type::comm_socket_type;

void on_accept(sever_socket_type &server, const comm_socket_type &comm_socket)
{
  UNUSED(server);
  UNUSED(comm_socket);
}

// void on_receive(sever_socket_type &                  server,
//                 const Eventr::tcp_server_socket::buffer_type buffer, const size_t &size)
// {
//   static int  count = 5;
//   std::string data(buffer.data(), size);
//   std::cout << "read : " << data << std::endl;

//   //   try
//   //   {
//   //     server.send(buffer.data(), size);
//   //   }
//   //   catch (std::exception &e)
//   //   {
//   //     std::cout << "exception: " << e.what() << std::endl;
//   //   }

//   if (count-- == 0)
//   {
//     server.stop();
//   }
// }

int main(int argc, char *argv[])
{
  uint32_t server_port = 5000;

  auto cli =
      lyra::cli_parser() | lyra::opt(server_port, "port")["-p"]["--port"]("TCP port to listen on");

  auto result = cli.parse({argc, argv});
  if (!result)
  {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    exit(1);
  }

  Eventr::io_handler io(10);
  sever_socket_type  server(io);

  server.start();

  server.bind("0.0.0.0", server_port);
  server.listen();
  server.set_on_accept(std::bind(on_accept, std::ref(server), std::placeholders::_1));
  //   server.set_on_receive(
  //       std::bind(on_receive, std::ref(server), std::placeholders::_1, std::placeholders::_2));

  io.run();

  server.stop();

  return 0;
}