#ifndef TCP_COMM_SOCKET_H
#define TCP_COMM_SOCKET_H

#include "eventr.h"
#include "itcp_socket.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace Eventr {
template <size_t SIZE>
class tcp_comm_socket
{
public:
  using buffer_type      = typename itcp_socket<SIZE>::buffer_type;
  using receive_cb_type  = typename itcp_socket<SIZE>::receive_cb_type;
  using connect_cb_type  = typename itcp_socket<SIZE>::connect_cb_type;

  tcp_comm_socket(io_handler &io, int fd = -1)
    : io(io)
  {
    if (fd != -1)
    {
      fd = ::socket(AF_INET, SOCK_STREAM, 0);
    }

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

  ~tcp_comm_socket()
  {
    ::close(fd);
  }

  void start()
  {
    io.add(fd, std::bind(&tcp_comm_socket<SIZE>::on_connect, this),
           std::bind(&tcp_comm_socket<SIZE>::on_error, this));
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

  void connect(const std::string &server_ip, const uint32_t &port)
  {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = ::htons(port);
    inet_aton(server_ip.c_str(), &server_addr.sin_addr);
    if (::connect(fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  void send(const char *payload, const size_t &size)
  {
    if (::send(fd, payload, size, 0) < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  void set_on_connect(const connect_cb_type &cb)
  {
    connect_cb = cb;
  }
  void set_on_receive(const receive_cb_type &cb)
  {
    receive_cb = cb;
  }

  void connected()
  {
    // change connect callback with receive callback
    io.add(fd, std::bind(&tcp_comm_socket<SIZE>::on_receive, this),
           std::bind(&tcp_comm_socket<SIZE>::on_error, this));
  }

private:
  void on_receive()
  {
    size_t bytes_received = ::recv(fd, recv_buffer.data(), SIZE, 0);
    if (bytes_received > 0)
    {
      receive_cb(recv_buffer, bytes_received);
    }
  }

  void on_error()
  {
    io.remove(fd);
  }

  void on_connect()
  {
    connected();
    connect_cb();
  }

private:
  io_handler &    io;
  int             fd;
  receive_cb_type receive_cb;
  connect_cb_type connect_cb;
  buffer_type     recv_buffer;
};
}  // namespace Eventr
#endif
