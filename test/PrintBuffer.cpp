#include "../src/PrintBuffer.hpp"
#include "catch.hpp"

using namespace yb;

TEST_CASE( "is empty after creating", "PrintBuffer.empty" ) {
    PrintBuffer pbuf;
    REQUIRE(pbuf == "");
    REQUIRE(pbuf.empty());
}

