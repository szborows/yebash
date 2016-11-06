#include "../src/LineBuffer.hpp"
#include "catch.hpp"

using namespace yb;

TEST_CASE( "can insert a char to the empty LineBuffer", "LineBuffer.insert" ) {
    auto testCharacter = [=] (char c) {
        LineBuffer buf;
        buf.insert(c);
        std::string result{buf.get()};
        REQUIRE(result == std::string{c});
    };
    std::string domain = "abcdefghijklmnopqrstuvwxyz01234567890-_";
    for_each(domain.begin(), domain.end(), testCharacter);
}

