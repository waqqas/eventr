#ifndef EVENT_H
#define EVENT_H

namespace Eventr {
class event
{
protected:
  int fd = -1;
  public:
  int get_descriptor(){
    return fd;
  }
};
}  // namespace Eventr

#endif
