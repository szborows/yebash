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
    EscapeCodeHandler escapeCodeHandler;
    Printer printer(output, escapeCodeHandler);
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
        EscapeCodeHandler escapeCodeHandler;
        Printer printer(output, escapeCodeHandler);

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
    EscapeCodeHandler escapeCodeHandler;
    Printer printer(output, escapeCodeHandler);

    auto character = 'a';
    auto result = yebash(suggestion, printer, character);

    REQUIRE(result == character);
    REQUIRE(output.str() == "bc2");

    tearDown();
}

unsigned char rollSuggestions(HistorySuggestion &suggestion, Printer &printer, int n) {
    unsigned char result;
    for (int i = 0; i < n; i++) {
        result = yebash(suggestion, printer, 0x06);
    }
    return result;
}

TEST_CASE( "Suggestions can be switched", "[basic.browsing_suggestions]" ) {

    History history = createHistory({"a", "ab", "abc", "abcd", "bcd"});
    HistorySuggestion suggestion(history);

    std::stringstream output;
    EscapeCodeHandler escapeCodeHandler;
    Printer printer(output, escapeCodeHandler);

    yebash(suggestion, printer, 'a');

    SECTION( "one switch" ) {
        auto result = rollSuggestions(suggestion, printer, 1);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbc");
    }

    SECTION( "two switches" ) {
        auto result = rollSuggestions(suggestion, printer, 2);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbcb");
    }

    SECTION( "third switch to empty suggestion" ) {
        auto result = rollSuggestions(suggestion, printer, 3);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbcb");
    }

    SECTION( "no more suggestions" ) {
        auto result = rollSuggestions(suggestion, printer, 4);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbcb");
    }

    SECTION( "go back to the first suggestion" ) {
        auto result = rollSuggestions(suggestion, printer, 5);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbcbbcd");
    }

    tearDown();
}


TEST_CASE( "Backspace invalidates suggestions", "[basic.backspace]" ) {

    History history = createHistory({"12345", "def", "trolo", "abc", "123"});
    HistorySuggestion suggestion(history);

    std::stringstream output;
    EscapeCodeHandler escapeCodeHandler;
    Printer printer(output, escapeCodeHandler);

    constexpr char backspace = 0x7f;

    yebash(suggestion, printer, '1');
    yebash(suggestion, printer, '2');
    yebash(suggestion, printer, '3');
    yebash(suggestion, printer, '4');
    REQUIRE(output.str() == "2335");

    yebash(suggestion, printer, backspace);
    REQUIRE(output.str() == "2335");

    yebash(suggestion, printer, '4');
    REQUIRE(output.str() == "23355");

    tearDown();
}


TEST_CASE( "Backspaces can't break yebash", "[basic.backspace_underflow]" ) {

    History history = createHistory({"12345", "xyz"});
    HistorySuggestion suggestion(history);

    std::stringstream output;
    EscapeCodeHandler escapeCodeHandler;
    Printer printer(output, escapeCodeHandler);

    constexpr char backspace = 0x7f;

    yebash(suggestion, printer, '1');
    REQUIRE(output.str() == "2345");

    for (int i = 0; i < 1 << 10; ++i) {
        yebash(suggestion, printer, backspace);
    }
    REQUIRE(output.str() == "2345");

    tearDown();
}
