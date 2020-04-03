#include "catch.hpp"

#include "eventr.h"

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
