// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#ifndef EVENTR_FD_READER_H
#define EVENTR_FD_READER_H

#include "iio_handler.h"
#include "ireader.h"

#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>

namespace Eventr {
template <size_t SIZE>
class fd_reader : public ireader<SIZE>
{

public:
  using read_cb_type = typename ireader<SIZE>::read_cb_type;
  using buffer_type  = typename ireader<SIZE>::buffer_type;

  fd_reader(iio_handler &io, int fd)
    : io(io)
    , fd(fd)
  {}

  void set_cb(const read_cb_type &callback) override
  {
    cb = callback;
  }

  void start() override
  {
    io.add(fd, std::bind(&fd_reader<SIZE>::on_read, this),
           std::bind(&fd_reader<SIZE>::on_error, this), EPOLLIN);
  }

  void stop() override
  {
    io.remove(fd);
  }

  void flush()
  {
    if (::fsync(fd) < 0)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

private:
  void on_read()
  {
    size_t bytes_read = ::read(fd, read_buffer.data(), SIZE);
    cb(read_buffer, bytes_read);
  }

  void on_error()
  {
    io.remove(fd);
  }

private:
  iio_handler &io;
  int          fd;
  read_cb_type cb;
  buffer_type  read_buffer;
};
}  // namespace Eventr
#endif
