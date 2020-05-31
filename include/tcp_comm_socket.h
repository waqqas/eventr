#ifndef TCP_COMM_SOCKET_H
#define TCP_COMM_SOCKET_H

#include "eventr.h"
#include "itcp_socket.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
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

  tcp_comm_socket(io_handler &io, int fd = -1)
    : _io(io)
    , _fd(fd)
    , _isConnected(false)
  {
    // open a new socket if not already provided
    if (_fd == -1)
    {
      _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    }

    // std::cout << "COMM opened: " << _fd << std::endl;

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
      // std::cout << "COMM closed: " << _fd << std::endl;
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
    // std::cout << "start: " << _fd << " connected :" << _isConnected << std::endl;
    if (_isConnected == false)
    {
      _io.add(_fd, std::bind(&tcp_comm_socket<SIZE>::on_connect, this),
              std::bind(&tcp_comm_socket<SIZE>::on_error, this));
    }
    else
    {
      _io.add(_fd, std::bind(&tcp_comm_socket<SIZE>::on_receive, this),
              std::bind(&tcp_comm_socket<SIZE>::on_error, this));
    }
  }

  void mark_as_connected()
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

private:
  void on_receive()
  {
    ssize_t bytes_received = ::recv(_fd, _recv_buffer.data(), SIZE, 0);
    // std::cout << "receveied: (" << bytes_received << ") " << std::endl;
    if (bytes_received > 0)
    {
      _receive_cb(_recv_buffer, bytes_received);
    }
    else
    {
      _error_cb(errno);
    }
  }

  void on_error(void)
  {
    _io.remove(_fd);
    _error_cb(errno);
  }

  void on_connect()
  {
    mark_as_connected();
    _connect_cb();
  }

private:
  io_handler &    _io;
  int             _fd;
  bool            _isConnected;
  receive_cb_type _receive_cb;
  connect_cb_type _connect_cb;
  error_cb_type   _error_cb;
  buffer_type     _recv_buffer;
};

}  // namespace Eventr
#endif
