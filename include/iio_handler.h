#ifndef EVENTR_IIO_HANDLER_H
#define EVENTR_IIO_HANDLER_H

#include <cstdint>
#include <functional>

namespace Eventr {

class iio_handler
{
public:
  using event_success_cb_type = std::function<void(void)>;
  using event_error_cb_type   = std::function<void(void)>;

  virtual void add(int fd, const event_success_cb_type &success_cb,
                   const event_error_cb_type &error_cb, const uint32_t& events) = 0;
  virtual void remove(int fd)                                                  = 0;
  virtual void run(void)                                                       = 0;
  virtual void poll(void)                                                      = 0;
  virtual bool is_pollable(void)                                               = 0;
  virtual ~iio_handler()
  {}
};
}  // namespace Eventr

#endif
