// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#include "catch.hpp"
#include "epoll_handler.h"

#include <iostream>
#include <unistd.h>
/*
As a user of the library
I want to be notified when an activity happens on an fd
so that I can process the activity
*/

SCENARIO("Run function returns immidiately if no fd to monitor", "[lib][epoll]")
{
  GIVEN("that no fd is added")
  {
    Eventr::epoll_handler io;

    WHEN("is_pollable() is called")
    {
      THEN("it returns false")
      {
        REQUIRE(io.is_pollable() == false);
      }
    }
    WHEN("run function is called")
    {
      THEN("it returns immidiately")
      {
        io.run();
        REQUIRE(true);
      }
    }
  }
}

SCENARIO("Run function blocks when there is atleast one fd to monitor", "[lib][epoll]")
{
  GIVEN("that when an fd is added to be monitored")
  {
    Eventr::epoll_handler io;
    bool                  callback_called = false;

    // stdout is always writable
    io.add(
        STDOUT_FILENO,
        [&]() {
          io.remove(STDOUT_FILENO);
          callback_called = true;
        },
        [&]() {}, EPOLLOUT);
    WHEN("run function is called")
    {
      // ::write(STDIN_FILENO, "\n", 1);
      THEN("it calls success callback")
      {
        io.run();
        REQUIRE(callback_called);
      }
    }
  }
}
