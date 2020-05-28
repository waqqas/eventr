#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H

#include "eventr.h"
#include "iudp_socket.h"

#include <arpa/inet.h>
#include <fcntl.h>
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

  udp_socket(io_handler &io)
    : io(io)
  {
    fd = ::socket(AF_INET, SOCK_DGRAM, 0);

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

  ~udp_socket()
  {
    ::close(fd);
  }

  void set_cb(const receive_cb_type &callback)
  {
    cb = callback;
  }

  void start()
  {
    io.add(fd, std::bind(&udp_socket<SIZE>::on_receive, this),
           std::bind(&udp_socket<SIZE>::on_error, this));
  }

  void stop()
  {
    io.remove(fd);
  }

  void bind(const std::string &server_ip, const uint32_t &port)
  {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);
    inet_aton(server_ip.c_str(), &server_addr.sin_addr);
    if (::bind(fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  void send(const char *payload, const size_t &size, const sockaddr_in &remote_addr)
  {
    socklen_t addr_length = sizeof(remote_addr);

    if (::sendto(fd, payload, size, 0, (sockaddr *)&remote_addr, addr_length) < 0)
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

private:
  void on_receive()
  {
    sockaddr_in addr;
    socklen_t   add_len = sizeof(addr);

    size_t bytes_received =
        ::recvfrom(fd, recv_buffer.data(), SIZE, 0, (sockaddr *)&addr, &add_len);
    if (bytes_received > 0)
    {
      cb(recv_buffer, bytes_received, addr);
    }
  }

  void on_error()
  {
    io.remove(fd);
  }

private:
  io_handler &    io;
  int             fd;
  receive_cb_type cb;
  buffer_type     recv_buffer;
};
}  // namespace Eventr
#endif
