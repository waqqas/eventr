#include "fd_reader.h"
#include "tcp_comm_socket.h"
#include "tcp_server_socket.h"

#include <csignal>
#include <functional>
#include <iostream>
#include <lyra/lyra.hpp>
#include <memory>
#include <tuple>
#include <unistd.h>
#include <unordered_map>
#include <utility>

#define UNUSED(x) (void)(x)

class App
{
  using server_socket_type = Eventr::tcp_server_socket<2048>;
  using comm_socket_type   = typename server_socket_type::comm_socket_type;
  using reader_type        = Eventr::fd_reader<2048>;

  struct client_data
  {
    int                               count = 0;
    std::unique_ptr<comm_socket_type> socket;
  };

  using client_list_type = std::unordered_map<int, client_data>;

public:
  void on_receive(const int id, const comm_socket_type::buffer_type &buffer, const ssize_t &size)
  {
    std::string data(buffer.data(), size);
    std::cout << "received : " << data << " on " << id << std::endl;

    auto it = client_list.find(id);
    if (it != client_list.end())
    {
      // echo back what is received
      it->second.socket->send(buffer.data(), size);
      if (it->second.count-- == 0)
      {
        client_list.erase(it);
      }
    }
  }
  void on_error(const int id, const int &error)
  {
    std::cout << "on_error: " << id << " error: " << ::strerror(error) << std::endl;

    auto it = client_list.find(id);
    if (it != client_list.end())
    {
      client_list.erase(it);
    }
  }

  void on_accept(comm_socket_type &comm_socket)
  {
    client_list_type::iterator it;
    bool                       inserted = false;

    std::tie(it, inserted) = client_list.insert(
        {comm_socket.id(),
         client_data{5, std::make_unique<comm_socket_type>(std::move(comm_socket))}});

    std::cout << "New client connected: " << it->second.socket->id() << std::endl;

    if (inserted == true)
    {
      it->second.socket->set_on_receive(std::bind(&App::on_receive, this, it->second.socket->id(),
                                                  std::placeholders::_1, std::placeholders::_2));
      it->second.socket->set_on_error(
          std::bind(&App::on_error, this, it->second.socket->id(), std::placeholders::_1));

      it->second.socket->start();
    }
  }

  void on_accept_error(const int &error)
  {
    std::cout << "on_accept_error: " << ::strerror(error) << std::endl;
  }

  void on_read(const reader_type::buffer_type &buffer, const size_t &size)
  {
    static int  count = 5;
    std::string data(buffer.data(), size);
    std::cout << "read : " << data << std::endl;

    if (data == "exit\n")
    {
      deinit();
    }
    else
    {
      for (auto &client : client_list)
      {
        std::cout << "sending on: " << client.second.socket->id() << std::endl;
        try
        {
          client.second.socket->send(buffer.data(), size);
        }
        catch (std::exception &e)
        {
          std::cout << "send exception: " << e.what() << std::endl;
        }
      }

      if (count-- == 0)
      {
        reader.stop();
      }
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
    server.set_on_error(std::bind(&App::on_accept_error, this, std::placeholders::_1));
    server.start();

    reader.set_cb(std::bind(&App::on_read, this, std::placeholders::_1, std::placeholders::_2));
    reader.start();
  }

  void deinit()
  {
    reader.stop();
    server.stop();
  }

private:
  server_socket_type server;
  reader_type        reader;
  client_list_type   client_list;
};

namespace {
volatile std::sig_atomic_t g_signal_status;
}

void handler(int sig)
{
  g_signal_status = sig;
}

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

  std::signal(SIGINT, handler);

  app.init(server_ip, server_port);

  while (io.is_pollable() && g_signal_status == 0)
  {
    io.poll();
  }

  std::cout << "app exiting" << std::endl;

  return 0;
}