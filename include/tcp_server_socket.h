#ifndef TCP_SERVER_SOCKET_H
#define TCP_SERVER_SOCKET_H

#include "eventr.h"
#include "tcp_comm_socket.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

namespace Eventr {

template <size_t SIZE>
class tcp_server_socket
{
public:
  using comm_socket_type = tcp_comm_socket<SIZE>;
  using accept_cb_type   = std::function<void(comm_socket_type &)>;

  tcp_server_socket(io_handler &io)
    : io(io)
  {
    fd = ::socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1)
    {
      throw std::runtime_error(::strerror(errno));
    }

    // make socket non-blocking
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
      throw std::runtime_error(::strerror(errno));
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  ~tcp_server_socket()
  {
    ::close(fd);
  }

  void start()
  {
    io.add(fd, std::bind(&tcp_server_socket::on_accept, this),
           std::bind(&tcp_server_socket::on_error, this));
  }

  void stop()
  {
    io.remove(fd);
  }

  void bind(const std::string &server_ip, const uint32_t &port)
  {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = ::htons(port);
    inet_aton(server_ip.c_str(), &server_addr.sin_addr);
    if (::bind(fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  void listen(int backlog = 10)
  {
    if (::listen(fd, backlog) < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  void set_on_accept(const accept_cb_type &cb)
  {
    accept_cb = cb;
  }

private:
  using comm_list_type = std::unordered_map<int, comm_socket_type>;

  void on_accept()
  {
    sockaddr_in client_addr;
    socklen_t   addr_length = sizeof(client_addr);
    int         comm_fd     = -1;

    comm_fd = ::accept(fd, (sockaddr *)&client_addr, &addr_length);
    if (comm_fd < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }

    // std::shared_ptr<comm_socket_type> comm_socket = std::make_shared<comm_socket_type>(io, fd);
    // comm_socket->mark_as_connected();  // already connected
    // accept_cb(comm_socket);

    comm_socket_type comm_socket(io, fd);
    comm_socket.mark_as_connected();

    typename comm_list_type::iterator it;
    bool                              inserted = false;
    std::tie(it, inserted)                     = comm_list.emplace(fd, std::move(comm_socket));

    if (inserted == true)
    {
      std::cout << "new client: " << it->second << std::endl;
      accept_cb(it->second);
    }
  }

  // void stop_comm_socket(const comm_socket_type &comm_socket)
  // {
  //   comm_list.find(comm_socket.fd);
  // }

  void on_error()
  {
    io.remove(fd);
  }

private:
  io_handler &   io;
  int            fd;
  accept_cb_type accept_cb;
  comm_list_type comm_list;
};
}  // namespace Eventr
#endif
