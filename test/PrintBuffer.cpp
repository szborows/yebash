#include "../src/PrintBuffer.hpp"
#include "catch.hpp"

using namespace yb;

TEST_CASE( "is empty after creating", "PrintBuffer.empty" ) {
    PrintBuffer pbuf;
    REQUIRE(pbuf == "");
    REQUIRE(pbuf.empty());
}

TEST_CASE( "does not return any value if empty", "PrintBuffer.noValueWhenEmpty" ) {
    PrintBuffer pbuf;
    for (int i = 0; i < 100; i++) {
        REQUIRE(pbuf.getNextChar() == CharOpt{});
    }
}

TEST_CASE( "can be assigned to the string", "PrintBuffer.stringMoveAndCopy" ) {
    PrintBuffer pbuf;
    pbuf = "Some string";
    REQUIRE(pbuf == "Some string");
    REQUIRE(pbuf.empty() == false);
    pbuf = std::string{"Some other string"};
    REQUIRE(pbuf == "Some other string");
    REQUIRE(pbuf.empty() == false);
    std::string testString{"Next string"};
    pbuf = testString;
    REQUIRE(pbuf == testString.c_str());
    REQUIRE(pbuf.empty() == false);
}

TEST_CASE( "can get chars out of PrintBuffer", "PrintBuffer.getChars" ) {
    PrintBuffer pbuf;
    std::string test{"test string"};
    pbuf = test;
    for_each(test.begin(), test.end(), [&](char c) {
        REQUIRE(pbuf.getNextChar().value() == c);
    });
    REQUIRE(pbuf.empty());
    REQUIRE(pbuf.getNextChar() == CharOpt{});
}

