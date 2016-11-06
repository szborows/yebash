#include "../src/ArrowHandler.hpp"
#include "../src/EscapeCodeGenerator.hpp"
#include "catch.hpp"
#include <string>

using namespace yb;

TEST_CASE( "no respone when on normal characters", "ArrowHandler.noResponse" ) {
    ANSIEscapeCodeGenerator gen;
    ArrowHandler handler(gen);
    auto testCharacter = [&] (const unsigned char c) {
        auto result = handler.handle(c);
        REQUIRE(result.operator bool() == false);
    };
    std::string domain = "abcdefghijklmnopqrstuvwxyz01234567890-_";
    for_each(begin(domain), end(domain), testCharacter);
}

