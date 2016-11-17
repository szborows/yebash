#include <ArrowHandler.hpp>
#include <EscapeCodeGenerator.hpp>
#include "catch.hpp"
#include <string>

using namespace yb;

namespace {

void testArrow(ArrowHandler &handler, char c, ArrowHandler::ArrowOpt a) {
    auto result = handler.handle(0x1b);
    REQUIRE(result.operator bool() == true);
    REQUIRE(result.value() == Arrow::invalid);
    result = handler.handle('[');
    REQUIRE(result.operator bool() == true);
    REQUIRE(result.value() == Arrow::invalid);
    result = handler.handle(c);
    REQUIRE(result == a);
}

} // namespace anon

TEST_CASE( "no respone when on normal characters", "[ArrowHandler.noResponse]" ) {
    ANSIEscapeCodeGenerator gen;
    ArrowHandler handler(gen);
    auto testCharacter = [&] (const unsigned char c) {
        auto result = handler.handle(c);
        REQUIRE(result.operator bool() == false);
    };
    std::string domain = "abcdefghijklmnopqrstuvwxyz01234567890-_";
    for_each(begin(domain), end(domain), testCharacter);
}

TEST_CASE( "proper respone on arrow escape code given", "[ArrowHandler.arrows]" ) {
    ANSIEscapeCodeGenerator gen;
    ArrowHandler handler(gen);
    SECTION( "up arrow" ) {
        testArrow(handler, 'A', Arrow::up);
    }
    SECTION( "down arrow" ) {
        testArrow(handler, 'B', Arrow::down);
    }
    SECTION( "right arrow" ) {
        testArrow(handler, 'C', Arrow::right);
    }
    SECTION( "left arrow" ) {
        testArrow(handler, 'D', Arrow::left);
    }
}

TEST_CASE( "gives proper responses on multiple arrows", "[ArrowHandler.multiple]" ) {
    ANSIEscapeCodeGenerator gen;
    ArrowHandler handler(gen);
    SECTION( "up arrows" ) {
        for (int i = 0; i < 100; i++) {
            testArrow(handler, 'A', Arrow::up);
        }
    }
    SECTION( "down arrows" ) {
        for (int i = 0; i < 100; i++) {
            testArrow(handler, 'B', Arrow::down);
        }
    }
    SECTION( "right arrows" ) {
        for (int i = 0; i < 100; i++) {
            testArrow(handler, 'C', Arrow::right);
        }
    }
    SECTION( "left arrows" ) {
        for (int i = 0; i < 100; i++) {
            testArrow(handler, 'D', Arrow::left);
        }
    }
	// TODO: add random arrows test
}

TEST_CASE( "returns nothing on bad data", "[ArrowHandler.bad]" ) {
    ANSIEscapeCodeGenerator gen;
    ArrowHandler handler(gen);
    std::string domain = "abcdefghijklmnopqrstuvwxyz01234567890-_";
    for (auto c : domain) {
        testArrow(handler, c, {});
    }
}

