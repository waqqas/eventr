// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#include "catch.hpp"
#include "epoll_handler.h"

#include <unistd.h>
/*
As a application that is using this library
I want to be notified when an activity happens on an fd
so that I can process the activity
*/

// Error callback is called
// Thread safety considerations
// Calling add again, updates the callbacks

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

SCENARIO("Success callback is called", "[lib][epoll]")
{
  GIVEN("that an fd is added to be monitored")
  {
    Eventr::epoll_handler io;
    bool                  callback_called = false;
    pthread_t             run_thread_id, callback_thread_id;

    // stdout is always writable
    io.add(
        STDOUT_FILENO,
        [&]() {
          callback_called    = true;
          callback_thread_id = pthread_self();

          io.remove(STDOUT_FILENO);
        },
        [&]() {}, EPOLLOUT);
    WHEN("run function is called")
    {
      THEN("it calls success callback, in the same thread")
      {
        run_thread_id = pthread_self();

        io.run();

        REQUIRE(callback_called);
        REQUIRE(pthread_equal(callback_thread_id, run_thread_id));
      }
    }
  }
}

SCENARIO("Error callback is called", "[lib][epoll]")
{
  GIVEN("that an fd is added to be monitored")
  {
    Eventr::epoll_handler io;
    bool                  callback_called = false;
    pthread_t             run_thread_id, callback_thread_id;

    ::close(STDIN_FILENO);
    io.add(
        STDIN_FILENO, [&]() {},
        [&]() {
          callback_called    = true;
          callback_thread_id = pthread_self();

          io.remove(STDIN_FILENO);
        },
        EPOLLIN);
    WHEN("run function is called")
    {
      THEN("it calls error callback, in the same thread")
      {
        run_thread_id = pthread_self();

        io.run();

        REQUIRE(callback_called);
        REQUIRE(pthread_equal(callback_thread_id, run_thread_id));
      }
    }
  }
}
