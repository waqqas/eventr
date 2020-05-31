#ifndef TCP_SERVER_SOCKET_H
#define TCP_SERVER_SOCKET_H

#include "eventr.h"
#include "tcp_comm_socket.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
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
  using error_cb_type    = std::function<void(void)>;

  tcp_server_socket(io_handler &io)
    : _io(io)
  {
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);

    if (_fd == -1)
    {
      throw std::runtime_error(::strerror(errno));
    }

    // make socket non-blocking
    int flags = fcntl(_fd, F_GETFL, 0);
    if (flags == -1)
    {
      throw std::runtime_error(::strerror(errno));
    }
    flags |= O_NONBLOCK;
    if (fcntl(_fd, F_SETFL, flags) == -1)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  ~tcp_server_socket()
  {
    ::close(_fd);
  }

  int id() const
  {
    return _fd;
  }

  void start()
  {
    _io.add(_fd, std::bind(&tcp_server_socket::on_accept, this),
            std::bind(&tcp_server_socket::on_error, this));
  }

  void stop()
  {
    _io.remove(_fd);
  }

  void bind(const std::string &server_ip, const uint32_t &port)
  {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);
    inet_aton(server_ip.c_str(), &server_addr.sin_addr);
    if (::bind(_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  void listen(int backlog = 10)
  {
    if (::listen(_fd, backlog) < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  void set_on_accept(const accept_cb_type &cb)
  {
    _accept_cb = cb;
  }

  void set_on_error(const error_cb_type &cb)
  {
    _error_cb = cb;
  }

  friend std::ostream &operator<<(std::ostream &os, const tcp_server_socket<SIZE> &socket)
  {
    os << "id:" << socket.id();
    return os;
  }

private:
  void on_accept()
  {
    sockaddr_in client_addr;
    socklen_t   addr_length = sizeof(client_addr);
    int         comm_fd     = -1;

    comm_fd = ::accept(_fd, (sockaddr *)&client_addr, &addr_length);
    if (comm_fd < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }

    comm_socket_type comm_socket(_io, comm_fd);
    comm_socket.mark_as_connected();
    _accept_cb(comm_socket);
  }

  void on_error()
  {
    _io.remove(_fd);
    _error_cb();
  }

private:
  io_handler &   _io;
  int            _fd;
  accept_cb_type _accept_cb;
  error_cb_type  _error_cb;
};
}  // namespace Eventr
#endif
