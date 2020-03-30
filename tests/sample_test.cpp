#include "catch.hpp"

SCENARIO( "Packer reception", "[network]" ) {

    GIVEN( "When a packet is scheduled to be received" ) {
        WHEN( "A packet is received" ) {
            THEN( "Packet reception callback is called" ) {
                REQUIRE(true);
            }
        }
    }
}
