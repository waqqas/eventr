#ifndef fd_reader_H
#define fd_reader_H

#include "eventr.h"

#include <functional>
#include <iostream>
#include <sys/timerfd.h>

namespace Eventr {
class fd_reader
{
public:
  using read_cb = std::function<void(void)>;

  fd_reader(io_handler &io, int fd)
    : io(io)
    , fd(fd)
  {}

  void set_cb(const read_cb &callback)
  {
    cb = callback;
  }

  void start()
  {
    io.add(fd, std::bind(&fd_reader::on_read, this), std::bind(&fd_reader::on_error, this));
  }

  void stop()
  {
    io.remove(fd);
  }

private:
  void on_read()
  {
    cb();
  }

  void on_error()
  {
    io.remove(fd);
  }

private:
  io_handler &io;
  int         fd;
  read_cb     cb;
};
}  // namespace Eventr
#endif
