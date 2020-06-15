// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#ifndef EVENTR_TCP_COMM_SOCKET_H
#define EVENTR_TCP_COMM_SOCKET_H

#include "iio_handler.h"
#include "itcp_socket.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

namespace Eventr {
template <size_t SIZE>
class tcp_comm_socket
{
public:
  using buffer_type     = typename itcp_socket<SIZE>::buffer_type;
  using receive_cb_type = typename itcp_socket<SIZE>::receive_cb_type;
  using connect_cb_type = typename itcp_socket<SIZE>::connect_cb_type;
  using error_cb_type   = typename itcp_socket<SIZE>::error_cb_type;

  tcp_comm_socket(iio_handler &io, int fd = -1)
    : _io(io)
    , _fd(fd)
    , _isConnected(false)
  {
    // open a new socket if not already provided
    if (_fd == -1)
    {
      _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    }

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

  // non-copyable
  tcp_comm_socket(const tcp_comm_socket &other) = delete;
  tcp_comm_socket &operator=(const tcp_comm_socket &other) = delete;

  tcp_comm_socket(tcp_comm_socket &&other)
    : _io(other._io)
    , _fd(other._fd)
    , _isConnected(other._isConnected)
  {
    other._fd = -1;
  }

  tcp_comm_socket &operator=(tcp_comm_socket &&other)
  {
    if (this != &other)
    {
      _io          = other._io;
      _fd          = other._fd;
      _isConnected = other._isConnected;
      other._fd    = -1;
    }
  }

  ~tcp_comm_socket()
  {
    if (_fd > -1)
    {
      stop();
      ::close(_fd);
    }
  }

  int id() const
  {
    return _fd;
  }

  void start()
  {
    if (_isConnected == false)
    {
      // wait for socket to become writable, which shows that client is connected
      _io.add(_fd, std::bind(&tcp_comm_socket<SIZE>::on_connect, this),
              std::bind(&tcp_comm_socket<SIZE>::on_error, this), EPOLLOUT);
    }
    else
    {
      _io.add(_fd, std::bind(&tcp_comm_socket<SIZE>::on_receive, this),
              std::bind(&tcp_comm_socket<SIZE>::on_error, this), EPOLLIN);
    }
  }

  void mark_as_connected() noexcept
  {
    _isConnected = true;
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

  void connect(const std::string &server_ip, const uint32_t &port)
  {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);
    inet_aton(server_ip.c_str(), &server_addr.sin_addr);
    if (::connect(_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
      if (errno != EINPROGRESS)
      {
        throw std::runtime_error(::strerror(errno));
      }
    }
    else
    {
      on_connect();
    }
  }

  void send(const char *payload, const size_t &size)
  {
    if (::send(_fd, payload, size, 0) < 0)
    {
      if (errno != EINPROGRESS)
      {
        throw std::runtime_error(::strerror(errno));
      }
    }
  }

  void set_on_connect(const connect_cb_type &cb)
  {
    _connect_cb = cb;
  }

  void set_on_receive(const receive_cb_type &cb)
  {
    _receive_cb = cb;
  }

  void set_on_error(const error_cb_type &cb)
  {
    _error_cb = cb;
  }

  friend std::ostream &operator<<(std::ostream &os, const tcp_comm_socket<SIZE> &socket)
  {
    os << "id:" << socket.id();
    return os;
  }

  template <typename T>
  void set_option(const int &option_name, const T &option_value, int level = SOL_SOCKET)
  {
    if (::setsockopt(_fd, level, option_name, (const void *)&option_value, sizeof(T)) < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  template <typename T>
  void get_option(const int &option_name, T &option_value, int level = SOL_SOCKET)
  {
    socklen_t option_len = sizeof(T);
    if (::getsockopt(_fd, level, option_name, (void *)&option_value, &option_len) < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

private:
  void on_receive()
  {
    ssize_t bytes_received = ::recv(_fd, _recv_buffer.data(), SIZE, 0);

    if (bytes_received > 0)
    {
      _receive_cb(_recv_buffer, bytes_received);
    }
    else
    {
      on_error();
    }
  }

  void on_error()
  {
    int       error;
    socklen_t result_len = sizeof(error);
    if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, &error, &result_len) >= 0)
    {
      _error_cb(error);
    }
  }

  void on_connect()
  {
    mark_as_connected();
    _connect_cb();
  }

private:
  iio_handler &   _io;
  int             _fd;
  bool            _isConnected;
  receive_cb_type _receive_cb;
  connect_cb_type _connect_cb;
  error_cb_type   _error_cb;
  buffer_type     _recv_buffer;
};

}  // namespace Eventr
#endif
