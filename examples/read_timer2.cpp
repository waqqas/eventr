#include "fd_reader.h"
#include "rtc_timer.h"

#include <iostream>
#include <lyra/lyra.hpp>
#include <string>

using reader_type = Eventr::fd_reader<1024>;

class App
{
public:
  void on_read(reader_type::buffer_type buffer, const size_t &size)
  {
    static int  count = 5;
    std::string data(buffer.data(), size);
    std::cout << "read : " << data << std::endl;
    if (count-- == 0)
    {
      reader.stop();
    }
  }

  void on_timer_expired(time_t expiry)
  {
    static int count = 5;
    std::cout << "timer expired: " << count << std::endl;
    if (count > 0)
    {
      count--;
      timer.expire_in(expiry);
    }
    else
    {
      timer.stop();
    }
  }

  App(Eventr::io_handler &io)
    : timer(io)
    , reader(io, STDERR_FILENO)
  {}

  void init()
  {
    timer.set_cb(std::bind(&App::on_timer_expired, this, 1));
    timer.expire_in(1);
    timer.start();

    reader.set_cb(std::bind(&App::on_read, this, std::placeholders::_1, std::placeholders::_2));
    reader.start();
  }

private:
  Eventr::rtc_timer timer;
  reader_type       reader;
};

int main()
{
  Eventr::io_handler io(10);
  App                app(io);

  app.init();

  io.run();

  return 0;
}