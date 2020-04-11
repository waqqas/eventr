#ifndef fd_reader_H
#define fd_reader_H

#include "eventr.h"
#include "ireader.h"

namespace Eventr {
template <size_t SIZE>
class fd_reader : public ireader<SIZE>
{

public:
  using read_cb_type = typename ireader<SIZE>::read_cb_type;
  using buffer_type  = typename ireader<SIZE>::buffer_type;

  fd_reader(io_handler &io, int fd)
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
           std::bind(&fd_reader<SIZE>::on_error, this));
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
  io_handler & io;
  int          fd;
  read_cb_type cb;
  buffer_type  read_buffer;
};
}  // namespace Eventr
#endif
