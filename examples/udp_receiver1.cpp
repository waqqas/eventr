#include "udp_receiver.h"

#include <iostream>

using receiver_type = Eventr::udp_receiver<2048>;

void on_receive(receiver_type &receiver, const receiver_type::buffer_type buffer, const size_t &size,
                const sockaddr_in &remote_addr)
{
  static int  count = 5;
  std::string data(buffer.data(), size);
  std::cout << "read : " << data << std::endl;

  receiver.send(buffer, size, remote_addr);

  if (count-- == 0)
  {
    receiver.stop();
  }
}

int main(void)
{
  Eventr::io_handler io(10);
  receiver_type      receiver(io);

  receiver.set_cb(std::bind(on_receive, std::ref(receiver), std::placeholders::_1,
                            std::placeholders::_2, std::placeholders::_3));
  receiver.bind("0.0.0.0", 5000);
  receiver.start();

  io.run();

  return 0;
}