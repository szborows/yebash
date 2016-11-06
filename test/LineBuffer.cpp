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

TEST_CASE( "can insert multiple chars", "LineBuffer.multiple" ) {
    LineBuffer buf;
    buf.insert('a');
    REQUIRE(std::string{buf.get()} == "a");
    buf.insert('b');
    REQUIRE(std::string{buf.get()} == "ab");
    buf.insert('c');
    REQUIRE(std::string{buf.get()} == "abc");
}

TEST_CASE( "remove can't break it", "LineBuffer.emptyRemove" ) {
    LineBuffer buf;
    for (int i = 0; i < 100; i++) {
        buf.remove();
        REQUIRE(std::string{buf.get()} == "");
    }
}

TEST_CASE( "can delete last character", "LineBuffer.deleteLast" ) {
    LineBuffer buf;
    buf.insert('a');
    buf.insert('b');
    buf.insert('c');
    buf.insert('d');
    buf.remove();
    REQUIRE(std::string{buf.get()} == "abc");
    buf.remove();
    REQUIRE(std::string{buf.get()} == "ab");
    buf.remove();
    REQUIRE(std::string{buf.get()} == "a");
}

TEST_CASE( "can move in buffer and insert/delete characters", "LineBuffer.move" ) {
    LineBuffer buf;
    buf.insert('a');
    buf.insert('b');
    buf.insert('c');
    buf.insert('d');
    buf.move(-1);
    buf.remove();
    REQUIRE(std::string{buf.get()} == "abd");
    buf.insert('z');
    REQUIRE(std::string{buf.get()} == "abzd");
    buf.insert('x');
    REQUIRE(std::string{buf.get()} == "abzxd");
    buf.move(1);
    buf.insert('c');
    REQUIRE(std::string{buf.get()} == "abzxdc");
}

