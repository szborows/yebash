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

