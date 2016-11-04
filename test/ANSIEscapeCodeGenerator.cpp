#include "../src/EscapeCodeGenerator.hpp"
#include "catch.hpp"

TEST_CASE( "does not generate escape code for moving forward by 0", "[ANSIEscapeCodeGenerator.nullMoveForward]") {
    ANSIEscapeCodeGenerator gen;
    auto result = gen.cursorForward(0);
    REQUIRE(result == "");
}

TEST_CASE( "can generate proper escape codes for moving forward", "[ANSIEscapeCodeGenerator.movingForward]") {
    ANSIEscapeCodeGenerator gen;
    auto test = [&] (int n) {
        std::string compare{"\033["};
        compare += std::to_string(n);
        compare += 'C';
        auto result = gen.cursorForward(n);
        REQUIRE(result == compare); 
    };
    for (int i = 1; i < 100; i++) {
        test(i);
    }
}

TEST_CASE( "does not generate escape code for moving backward by 0", "[ANSIEscapeCodeGenerator.nullMoveBackward]") {
    ANSIEscapeCodeGenerator gen;
    auto result = gen.cursorBackward(0);
    REQUIRE(result == "");
}

TEST_CASE( "can generate proper escape codes for moving backward", "[ANSIEscapeCodeGenerator.movingBackward]") {
    ANSIEscapeCodeGenerator gen;
    auto test = [&] (int n) {
        std::string compare{"\033["};
        compare += std::to_string(n);
        compare += 'D';
        auto result = gen.cursorBackward(n);
        REQUIRE(result == compare); 
    };
    for (int i = 1; i < 100; i++) {
        test(i);
    }
}

TEST_CASE( "generates proper escape code for clearing the line", "ANSIEscapeCodeGenerator.clearTerminalLine") {
    ANSIEscapeCodeGenerator gen;
    auto result = gen.clearTerminalLine();
    REQUIRE(result == "\033[K");
}

