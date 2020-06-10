// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#include "fd_reader.h"
#include "epoll_handler.h"
#include "tcp_comm_socket.h"

#include <iostream>
#include <lyra/lyra.hpp>
#include <string>
#include <thread>

#define UNUSED(x) (void)(x)

class App
{
  using comm_socket_type = Eventr::tcp_comm_socket<2048>;
  using reader_type      = Eventr::fd_reader<2048>;

public:
  void on_receive(const comm_socket_type::buffer_type &buffer, const ssize_t &size)
  {
    std::string data(buffer.data(), size);
    std::cout << "received : " << data << std::endl;
  }

  void on_error(const int &error)
  {
    std::cout << "on_error: " << ::strerror(error) << std::endl;
  }

  void on_connect()
  {
    std::cout << "on_connect: " << client << std::endl;
    client.set_on_receive(
        std::bind(&App::on_receive, this, std::placeholders::_1, std::placeholders::_2));
    client.set_on_error(std::bind(&App::on_error, this, std::placeholders::_1));
    client.start();
  }

  void on_connect_error(const int &error)
  {
    std::cout << "on_connect_error: " << client << " error:" << ::strerror(error) << std::endl;
  }

  void on_read(const reader_type::buffer_type &buffer, const size_t &size)
  {
    std::string data(buffer.data(), size);
    std::cout << "read : " << data << std::endl;

    client.send(buffer.data(), size);
  }

  App(Eventr::epoll_handler &io)
    : client(io)
    , reader(io, STDERR_FILENO)
  {}

  void init(const std::string &server_ip, const uint32_t &server_port)
  {
    client.set_on_connect(std::bind(&App::on_connect, this));
    client.set_on_error(std::bind(&App::on_connect_error, this, std::placeholders::_1));

    client.start();
    client.connect(server_ip, server_port);

    reader.set_cb(std::bind(&App::on_read, this, std::placeholders::_1, std::placeholders::_2));
    reader.start();
  }

private:
  comm_socket_type client;
  reader_type      reader;
};

int main(int argc, char *argv[])
{
  uint32_t    server_port = 5000;
  std::string server_ip   = "127.0.0.1";

  auto cli = lyra::cli_parser() |
             lyra::opt(server_port, "port")["-p"]["--port"]("TCP port to connect to") |
             lyra::opt(server_ip, "ip")["-i"]["--ip"]("TCP IP address to connect to");
  ;

  auto result = cli.parse({argc, argv});
  if (!result)
  {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    exit(1);
  }

  Eventr::epoll_handler io(10);
  App                app(io);
  app.init(server_ip, server_port);

  try
  {
    io.run();
  }
  catch (std::exception &e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }

  std::cout << "exiting" << std::endl;

  return 0;
}