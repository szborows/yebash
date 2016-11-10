#include "../src/EscapeCodeGenerator.hpp"
#include "catch.hpp"

using namespace yb;

void testEscapeCodeWithNumber(const std::string &result, int n, char c) {
    std::string compare{"\033["};
    compare += std::to_string(n) + c;
    REQUIRE(result == compare); 
}

TEST_CASE( "does not generate escape code for moving up by 0", "[ANSIEscapeCodeGenerator.nullMoveUp]" ) {
    ANSIEscapeCodeGenerator gen;
    auto result = gen.cursorUp(0);
    REQUIRE(result == "");
}

TEST_CASE( "can generate proper escape codes for moving up", "[ANSIEscapeCodeGenerator.movingUp]" ) {
    ANSIEscapeCodeGenerator gen;
    for (int i = 2; i < 100; i++) {
        testEscapeCodeWithNumber(gen.cursorUp(i), i, 'A');
    }
}

TEST_CASE( "does not generate escape code for moving down by 0", "[ANSIEscapeCodeGenerator.nullMoveDown]" ) {
    ANSIEscapeCodeGenerator gen;
    auto result = gen.cursorDown(0);
    REQUIRE(result == "");
}

TEST_CASE( "can generate proper escape codes for moving down", "[ANSIEscapeCodeGenerator.movingDown]" ) {
    ANSIEscapeCodeGenerator gen;
    for (int i = 2; i < 100; i++) {
        testEscapeCodeWithNumber(gen.cursorDown(i), i, 'B');
    }
}

TEST_CASE( "does not generate escape code for moving forward by 0", "[ANSIEscapeCodeGenerator.nullMoveForward]" ) {
    ANSIEscapeCodeGenerator gen;
    auto result = gen.cursorForward(0);
    REQUIRE(result == "");
}

TEST_CASE( "can generate proper escape codes for moving forward", "[ANSIEscapeCodeGenerator.movingForward]" ) {
    ANSIEscapeCodeGenerator gen;
    for (int i = 2; i < 100; i++) {
        testEscapeCodeWithNumber(gen.cursorForward(i), i, 'C');
    }
}

TEST_CASE( "does not generate escape code for moving backward by 0", "[ANSIEscapeCodeGenerator.nullMoveBackward]" ) {
    ANSIEscapeCodeGenerator gen;
    auto result = gen.cursorBackward(0);
    REQUIRE(result == "");
}

TEST_CASE( "can generate proper escape codes for moving backward", "[ANSIEscapeCodeGenerator.movingBackward]" ) {
    ANSIEscapeCodeGenerator gen;
    for (int i = 2; i < 100; i++) {
        testEscapeCodeWithNumber(gen.cursorBackward(i), i, 'D');
    }
}

TEST_CASE( "generates proper escape code for clearing the line", "[ANSIEscapeCodeGenerator.clearTerminalLine]" ) {
    ANSIEscapeCodeGenerator gen;
    auto result = gen.clearTerminalLine();
    REQUIRE(result == "\033[K");
}

TEST_CASE( "generates proper escape codes for colors", "[ANSIEscapeCodeGenerator.colors]" ) {
    ANSIEscapeCodeGenerator gen;
    for (int i = 0; i < 98; i++) {
        testEscapeCodeWithNumber(gen.setColor(static_cast<Color>(i)), i, 'm');
    }
}

