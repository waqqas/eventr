// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#ifndef EVENTR_IUDP_SOCKET_H
#define EVENTR_IUDP_SOCKET_H

#include <array>
#include <functional>
#include <netinet/in.h>
#include <string>

namespace Eventr {
template <size_t SIZE>
class iudp_socket
{
public:
  using buffer_type = std::array<char, SIZE>;
  using receive_cb_type =
      std::function<void(const buffer_type &, const size_t &, const sockaddr_in &)>;

  // virtual void set_cb(const receive_cb_type &)                         = 0;
  virtual void start()                                                 = 0;
  virtual void stop()                                                  = 0;
  virtual void bind(const std::string &, const uint32_t &)             = 0;
  virtual void send(const char *, const size_t &, const sockaddr_in &) = 0;
  virtual ~iudp_socket()
  {}
};
}  // namespace Eventr

#endif
