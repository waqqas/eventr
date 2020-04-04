#ifndef fd_reader_H
#define fd_reader_H

#include "eventr.h"

#include <array>
#include <functional>

namespace Eventr {
template <size_t SIZE>
class fd_reader
{
public:
  using buffer_type  = std::array<char, SIZE>;
  using read_cb_type = std::function<void(const buffer_type&, size_t)>;

  fd_reader(io_handler &io, int fd)
    : io(io)
    , fd(fd)
  {}

  void set_cb(const read_cb_type &callback)
  {
    cb = callback;
  }

  void start()
  {
    io.add(fd, std::bind(&fd_reader<SIZE>::on_read, this),
           std::bind(&fd_reader<SIZE>::on_error, this));
  }

  void stop()
  {
    io.remove(fd);
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
  io_handler & io;
  int          fd;
  read_cb_type cb;
  buffer_type  read_buffer;
};
}  // namespace Eventr
#endif
