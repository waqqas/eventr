#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H

#include "eventr.h"

#include <array>
#include <fcntl.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace Eventr {
template <size_t SIZE>
class udp_receiver
{
public:
  using buffer_type     = std::array<char, SIZE>;
  using receive_cb_type = std::function<void(const buffer_type &, size_t)>;
  udp_receiver(io_handler &io)
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
    if (fcntl(fd, F_SETFL, flags) == 0)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  ~udp_receiver()
  {
    ::close(fd);
  }

  void set_cb(const receive_cb_type &callback)
  {
    cb = callback;
  }

  void start()
  {
    io.add(fd, std::bind(&udp_receiver<SIZE>::on_receive, this),
           std::bind(&udp_receiver<SIZE>::on_error, this));
  }

  void stop()
  {
    io.remove(fd);
  }

  void bind(std::string ip, uint32_t port)
  {}

private:
  void on_receive()
  {
    sockaddr_in addr;
    socklen_t   add_len = sizeof(addr);

    size_t bytes_received =
        ::recvfrom(fd, recv_buffer.data(), SIZE, 0, (sockaddr *)&addr, &add_len);
    if (bytes_received > 0)
    {
      cb(recv_buffer, bytes_received);
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
