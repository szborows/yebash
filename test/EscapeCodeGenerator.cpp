#include "../src/EscapeCodeGenerator.hpp"
#include "catch.hpp"

using namespace yb;

TEST_CASE( "generates nothing", "EscapeCodeGenerator.nothing" ) {
    EscapeCodeGenerator gen;
    for (int i = 0; i < 100; i++)
        REQUIRE(gen.cursorUp(i) == "");
    for (int i = 0; i < 100; i++)
        REQUIRE(gen.cursorDown(i) == "");
    for (int i = 0; i < 100; i++)
        REQUIRE(gen.cursorForward(i) == "");
    for (int i = 0; i < 100; i++)
        REQUIRE(gen.cursorBackward(i) == "");
    for (int i = 0; i < 98; i++)
        REQUIRE(gen.setColor(static_cast<Color>(i)) == "");
    REQUIRE(gen.clearTerminalLine() == "");
}

