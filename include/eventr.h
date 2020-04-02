#ifndef EVENTR_H
#define EVENTR_H

#include <cerrno>
#include <sys/epoll.h>
#include <system_error>
#include <unistd.h>

namespace Eventr {

class io_handler
{
public:
  ~io_handler()
  {
    close();
  }

  void init(void)
  {
    epoll_fd = ::epoll_create1(0);
    if (epoll_fd == -1)
    {
      throw std::system_error();
    }
  }

  void close(void)
  {
    if (epoll_fd != -1)
    {
      ::close(epoll_fd);
      epoll_fd = -1;
    }
  }

private:
  int epoll_fd = -1;
};
}  // namespace Eventr

#endif