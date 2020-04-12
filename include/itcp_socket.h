#ifndef ITCP_SOCKET_H
#define IUPD_SOCKET_H

#include <array>
#include <functional>
#include <netinet/in.h>
#include <string>

namespace Eventr {
template <size_t SIZE>
class itcp_socket
{
public:
  using buffer_type     = std::array<char, SIZE>;
  using receive_cb_type = std::function<void(const buffer_type &, const size_t &)>;
  using connect_cb_type = std::function<void(void)>;

  virtual void start()                            = 0;
  virtual void stop()                             = 0;
  virtual void send(const char *, const size_t &) = 0;
  virtual ~itcp_socket()
  {}
};
}  // namespace Eventr

#endif