#ifndef EVENTR_H
#define EVENTR_H

#include <cerrno>
#include <functional>
// #include <iostream>
#include <stdexcept>
#include <string.h>
#include <sys/epoll.h>
#include <tuple>
#include <unistd.h>
#include <unordered_map>

namespace Eventr {

class io_handler
{
private:
  using event_success_cb_type = std::function<void(void)>;
  using event_error_cb_type   = std::function<void(void)>;

  struct event_data
  {
    int                   fd;
    event_success_cb_type success_cb;
    event_error_cb_type   error_cb;
  };

  using event_data_list_type = std::unordered_map<int, event_data>;
  using event_data_size_type = event_data_list_type::size_type;

  using epoll_list_type      = std::vector<epoll_event>;
  using epoll_list_data_size = epoll_list_type::size_type;

public:
  io_handler(epoll_list_data_size epoll_size = 1, event_data_size_type data_size = 0)
    : _event_list(data_size)
    , _epoll_list(epoll_size)
  {
    init();
  }

  ~io_handler()
  {
    close();
  }

  void add(int fd, const event_success_cb_type &success_cb, const event_error_cb_type &error_cb)
  {
    event_data_list_type::iterator it;
    bool                           inserted = false;

    std::tie(it, inserted) = _event_list.emplace(fd, event_data{fd, success_cb, error_cb});

    // std::cout << "adding: " << fd << " inserted:" << inserted << std::endl;

    if (inserted == true)
    {
      epoll_event ev;

      ev.data.ptr = &(it->second);
      ev.events   = EPOLLIN | EPOLLET;

      if (::epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
      {
        _event_list.erase(it);
        throw std::runtime_error(::strerror(errno));
      }
    }
    else
    {
      it->second = {fd, success_cb, error_cb};
    }
  }

  void remove(int fd)
  {
    // std::cout << "removing: " << fd << std::endl;

    if (_event_list.find(fd) != _event_list.end())
    {
      _event_list.erase(fd);

      if (::epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
      {
        throw std::runtime_error(::strerror(errno));
      }
    }
  }

  void run()
  {
    while (!_event_list.empty())
    {
      poll();
    }
  }

  void poll()
  {
    int event_count = ::epoll_wait(_epoll_fd, &_epoll_list[0], _epoll_list.size(), -1);
    for (int count = 0; count < event_count; count++)
    {
      event_data *data = (event_data *)_epoll_list[count].data.ptr;

      if ((_epoll_list[count].events & EPOLLERR) || (_epoll_list[count].events & EPOLLHUP) ||
          (!(_epoll_list[count].events & EPOLLIN)))
      {
        // std::cout << "calling error cb" << std::endl;
        data->error_cb();
        continue;
      }
      // std::cout << "calling success cb" << std::endl;

      data->success_cb();
    }
  }

private:
  void init(void)
  {
    _epoll_fd = ::epoll_create1(0);
    if (_epoll_fd == -1)
    {
      throw std::runtime_error(strerror(errno));
    }
  }

  void close(void)
  {
    if (_epoll_fd != -1)
    {
      ::close(_epoll_fd);
      _epoll_fd = -1;
    }
  }

private:
  int                  _epoll_fd = -1;
  event_data_list_type _event_list;
  epoll_list_type      _epoll_list;
};
}  // namespace Eventr

#endif