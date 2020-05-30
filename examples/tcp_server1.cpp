#include "fd_reader.h"
#include "tcp_comm_socket.h"
#include "tcp_server_socket.h"

#include <iostream>
#include <lyra/lyra.hpp>
#include <tuple>
#include <unordered_map>
#include <utility>

#define UNUSED(x) (void)(x)

class App
{
  using server_socket_type = Eventr::tcp_server_socket<2048>;
  using comm_socket_type   = typename server_socket_type::comm_socket_type;
  using reader_type        = Eventr::fd_reader<2048>;
  using client_list_type   = std::unordered_map<int, comm_socket_type>;

public:
  void on_receive(const int id, const comm_socket_type::buffer_type &buffer, const ssize_t &size)
  {
    static int  count = 5;
    std::string data(buffer.data(), size);
    std::cout << "received : " << data << " on " << id << std::endl;

    auto it = client_list.find(id);
    if (it != client_list.end())
    {
      // echo back what is received
      it->second.send(buffer.data(), size);
      if (count-- == 0)
      {
        it->second.stop();
      }
    }
  }

  void on_accept(const comm_socket_type &comm_socket)
  {
    std::cout << "New client connected: " << comm_socket << std::endl;

    client_list_type::iterator it;
    bool                       inserted = false;

    // std::tie(it, inserted) = client_list.emplace(comm_socket.id(), std::move(comm_socket));

    if (inserted == true)
    {
      // it->second.set_on_receive(std::bind(&App::on_receive, this, it->second.id(),
      //                                     std::placeholders::_1, std::placeholders::_2));
    }
  }

  void on_read(reader_type::buffer_type buffer, const size_t &size)
  {
    static int  count = 5;
    std::string data(buffer.data(), size);
    std::cout << "read : " << data << std::endl;
    if (count-- == 0)
    {
      reader.stop();
    }
  }

  App(Eventr::io_handler &io)
    : server(io)
    , reader(io, STDERR_FILENO)
  {}

  void init(const std::string &ip, uint32_t port)
  {
    server.bind(ip, port);
    server.listen();
    server.set_on_accept(std::bind(&App::on_accept, this, std::placeholders::_1));
    server.start();

    reader.set_cb(std::bind(&App::on_read, this, std::placeholders::_1, std::placeholders::_2));
    reader.start();
  }

private:
  server_socket_type server;
  reader_type        reader;
  client_list_type   client_list;
};

int main(int argc, char *argv[])
{
  uint32_t    server_port = 5000;
  std::string server_ip   = "0.0.0.0";

  auto cli = lyra::cli_parser() |
             lyra::opt(server_port, "port")["-p"]["--port"]("TCP port to listen on") |
             lyra::opt(server_ip, "ip")["-i"]["--ip"]("Local IP address");
  ;

  auto result = cli.parse({argc, argv});
  if (!result)
  {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    exit(1);
  }

  Eventr::io_handler io(10);
  App                app(io);

  app.init(server_ip, server_port);

  io.run();

  // app.stop();

  return 0;
}