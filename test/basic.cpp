#include "yebash.hpp"
#include "History.hpp"

#include "catch.hpp"
#include <initializer_list>
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace yb;
using namespace std;

namespace {

History createHistory(initializer_list<string> const& commands) {
    stringstream ss;
    for (auto && command: commands) {
        ss << command << std::endl;
    }

    History history;
    history.read(ss);
    return history;
}

void tearDown() {
    std::stringstream output, ss;
    Printer printer(output);
    History history;
    history.read(ss);
    HistorySuggestion suggestion(history);
    yebash(suggestion, printer, '\n');
}
} // anon namespace

TEST_CASE( "No suggestions when history is empty", "[basic.empty_history]"  ) {

    auto testCharacter = [] (char const c) {
        History history = createHistory({});
        HistorySuggestion suggestion(history);

        std::stringstream output;
        Printer printer(output);

        auto result = yebash(suggestion, printer, c);

        REQUIRE(result == c);
        REQUIRE(output.str() == "");
    };

    string domain = "abcdefghijklmnopqrstuvwxyz01234567890-_";
    for_each(begin(domain), end(domain), testCharacter);

    tearDown();
}


TEST_CASE( "Order of commands from history is preserved", "[basic.history_order_preserved]"  ) {

    History history = createHistory({"abc1", "abc2"});
    HistorySuggestion suggestion(history);

    std::stringstream output;
    Printer printer(output);

    auto character = 'a';
    auto result = yebash(suggestion, printer, character);

    REQUIRE(result == character);
    REQUIRE(output.str() == "bc2");

    tearDown();
}

TEST_CASE( "Suggestions can be switched", "[basic.browsing_suggestions]" ) {

    History history = createHistory({"a", "ab", "abc", "abcd", "bcd"});
    HistorySuggestion suggestion(history);

    std::stringstream output;
    Printer printer(output);

    yebash(suggestion, printer, 'a');

    SECTION( "one switch" ) {
        auto result = yebash(suggestion, printer, 0x06);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbc");
    }

    SECTION( "two switches" ) {
        yebash(suggestion, printer, 0x06);
        auto result = yebash(suggestion, printer, 0x06);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbcb");
    }

    SECTION( "third switch to empty suggestion" ) {
        yebash(suggestion, printer, 0x06);
        yebash(suggestion, printer, 0x06);
        auto result = yebash(suggestion, printer, 0x06);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbcb");
    }

    SECTION( "nothing more suggestions" ) {
        yebash(suggestion, printer, 0x06);
        yebash(suggestion, printer, 0x06);
        yebash(suggestion, printer, 0x06);
        auto result = yebash(suggestion, printer, 0x06);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbcb");
    }

    SECTION( "go back to the first suggestion" ) {
        yebash(suggestion, printer, 0x06);
        yebash(suggestion, printer, 0x06);
        yebash(suggestion, printer, 0x06);
        yebash(suggestion, printer, 0x06);
        auto result = yebash(suggestion, printer, 0x06);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbcbbcd");
    }

    tearDown();
}
