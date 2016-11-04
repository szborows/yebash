#include "../src/EscapeCodeGenerator.hpp"
#include "catch.hpp"

void testEscapeCodeWithNumber(const std::string &result, int n, char c) {
    std::string compare{"\033["};
    compare += std::to_string(n);
    compare += c;
    REQUIRE(result == compare); 
}

TEST_CASE( "does not generate escape code for moving forward by 0", "[ANSIEscapeCodeGenerator.nullMoveForward]") {
    ANSIEscapeCodeGenerator gen;
    auto result = gen.cursorForward(0);
    REQUIRE(result == "");
}

TEST_CASE( "can generate proper escape codes for moving forward", "[ANSIEscapeCodeGenerator.movingForward]") {
    ANSIEscapeCodeGenerator gen;
    for (int i = 1; i < 100; i++) {
        testEscapeCodeWithNumber(gen.cursorForward(i), i, 'C');
    }
}

TEST_CASE( "does not generate escape code for moving backward by 0", "[ANSIEscapeCodeGenerator.nullMoveBackward]") {
    ANSIEscapeCodeGenerator gen;
    auto result = gen.cursorBackward(0);
    REQUIRE(result == "");
}

TEST_CASE( "can generate proper escape codes for moving backward", "[ANSIEscapeCodeGenerator.movingBackward]") {
    ANSIEscapeCodeGenerator gen;
    for (int i = 1; i < 100; i++) {
        testEscapeCodeWithNumber(gen.cursorBackward(i), i, 'D');
    }
}

TEST_CASE( "generates proper escape code for clearing the line", "ANSIEscapeCodeGenerator.clearTerminalLine") {
    ANSIEscapeCodeGenerator gen;
    auto result = gen.clearTerminalLine();
    REQUIRE(result == "\033[K");
}

TEST_CASE( "generates proper escape codes for colors", "ANSIEscapeCodeGenerator.colors") {
    ANSIEscapeCodeGenerator gen;
    for (int i = 0; i < 98; i++) {
        testEscapeCodeWithNumber(gen.setColor(static_cast<Color>(i)), i, 'm');
    }
}

