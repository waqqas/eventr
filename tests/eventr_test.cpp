// Copyright 2020 - 2020, Waqqas Jabbar
// SPDX-License-Identifier: BSD-3-Clause

#include "catch.hpp"

#include "io_handler.h"

SCENARIO("Initialization", "[lib][init]")
{
  GIVEN("")
  {
    WHEN("")
    {
      Eventr::io_handler io;
      THEN("")
      {
        io.run();
        REQUIRE(true);
      }
    }
  }
}
