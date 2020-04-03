#ifndef EVENTR_H
#define EVENTR_H

#include <cerrno>
#include <functional>
#include <list>
#include <stdexcept>
#include <string.h>
#include <sys/epoll.h>
#include <system_error>
#include <unistd.h>

namespace Eventr {

class io_handler
{
private:
  using event_cb = std::function<void(int)>;

  struct event_data
  {
    int      fd;
    event_cb cb;
  };

  using event_data_list_type = std::list<event_data>;
  using event_data_size_type = event_data_list_type::size_type;

  using epoll_list_type      = std::vector<epoll_event>;
  using epoll_list_data_size = epoll_list_type::size_type;

public:
  io_handler(event_data_size_type data_size = 0, epoll_list_data_size epoll_size = 0)
    : event_list(data_size)
    , epoll_list(epoll_size)
  {}

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

  void add(int fd, event_cb cb)
  {
    event_list.push_back({fd, cb});

    epoll_event ev;

    ev.data.ptr = &event_list.back();
    ev.events   = EPOLLIN | EPOLLET;

    int retval = ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if (retval == -1)
    {
      event_list.pop_back();
      throw std::runtime_error(::strerror(errno));
    }
  }

  void remove(int fd)
  {
    std::remove_if(event_list.begin(), event_list.end(),
                   [&fd](event_data &ed) { return (ed.fd == fd); });

    int retval = ::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    if (retval == -1)
    {
      throw std::runtime_error(::strerror(errno));
    }
  }

  void run(void)
  {
    int event_count = ::epoll_wait(epoll_fd, &epoll_list[0], epoll_list.max_size(), -1);
    for (int count = 0; count < event_count; count++)
    {
      event_data *data = (event_data *)epoll_list[count].data.ptr;
      data->cb(data->fd);
    }
  }

private:
  int                  epoll_fd = -1;
  event_data_list_type event_list;
  epoll_list_type      epoll_list;
};
}  // namespace Eventr

#endif