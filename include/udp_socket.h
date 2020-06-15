// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#ifndef EVENTR_UDP_RECEIVER_H
#define EVENTR_UDP_RECEIVER_H

#include "iio_handler.h"
#include "iudp_socket.h"

#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace Eventr {
template <size_t SIZE>
class udp_socket : public iudp_socket<SIZE>
{
public:
  using buffer_type     = typename iudp_socket<SIZE>::buffer_type;
  using receive_cb_type = typename iudp_socket<SIZE>::receive_cb_type;

  udp_socket(iio_handler &io)
    : _io(io)
  {
    _fd = ::socket(AF_INET, SOCK_DGRAM, 0);

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

  ~udp_socket()
  {
    ::close(_fd);
  }

  void set_cb(const receive_cb_type &callback)
  {
    _cb = callback;
  }

  void start()
  {
    _io.add(_fd, std::bind(&udp_socket<SIZE>::on_receive, this),
           std::bind(&udp_socket<SIZE>::on_error, this), EPOLLIN);
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

  void send(const char *payload, const size_t &size, const sockaddr_in &remote_addr)
  {
    socklen_t addr_length = sizeof(remote_addr);

    if (::sendto(_fd, payload, size, 0, (sockaddr *)&remote_addr, addr_length) < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  void send(const char *payload, const size_t &size, const std::string &to_ip,
            const uint32_t &to_port)
  {
    sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port   = htons(to_port);
    inet_aton(to_ip.c_str(), &remote_addr.sin_addr);

    send(payload, size, remote_addr);
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
    sockaddr_in addr;
    socklen_t   add_len = sizeof(addr);

    size_t bytes_received =
        ::recvfrom(_fd, _recv_buffer.data(), SIZE, 0, (sockaddr *)&addr, &add_len);
    if (bytes_received > 0)
    {
      _cb(_recv_buffer, bytes_received, addr);
    }
  }

  void on_error()
  {
    _io.remove(_fd);
  }

private:
  iio_handler &   _io;
  int             _fd;
  receive_cb_type _cb;
  buffer_type     _recv_buffer;
};
}  // namespace Eventr
#endif
