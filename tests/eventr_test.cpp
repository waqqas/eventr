// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#include "catch.hpp"

#include "epoll_handler.h"

SCENARIO("Initialization", "[lib][init]")
{
  GIVEN("")
  {
    WHEN("")
    {
      Eventr::epoll_handler io;
      THEN("")
      {
        io.run();
        REQUIRE(true);
      }
    }
  }
}
