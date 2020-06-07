#ifndef EVENTR_IREADER_H
#define EVENTR_IREADER_H

#include <array>
#include <functional>

namespace Eventr {
template <size_t SIZE>
class ireader
{
public:
  using buffer_type  = std::array<char, SIZE>;
  using read_cb_type = std::function<void(const buffer_type &, const size_t &)>;

  virtual void set_cb(const read_cb_type &) = 0;
  virtual void start()                      = 0;
  virtual void stop()                       = 0;
  virtual ~ireader()
  {}
};
}  // namespace Eventr

#endif
