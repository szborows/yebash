#include "../src/yebash.hpp"
#include "../src/History.hpp"

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
    EscapeCodeGenerator escapeCodeGenerator;
    Printer printer(output, escapeCodeGenerator);
    History history;
    history.read(ss);
    LineBuffer buf;
    ArrowHandler arrowHandler(escapeCodeGenerator);
    PrintBuffer printBuffer;
    HistorySuggestion suggestion(history);
    yebash(suggestion, printer, buf, printBuffer, arrowHandler, '\n');
}
} // anon namespace

TEST_CASE( "No suggestions when history is empty", "[basic.empty_history]"  ) {

    auto testCharacter = [] (char const c) {
        History history = createHistory({});
        HistorySuggestion suggestion(history);
        PrintBuffer printBuffer;
        EscapeCodeGenerator escapeCodeGenerator;
        ArrowHandler arrowHandler(escapeCodeGenerator);
        std::stringstream output;
        Printer printer(output, escapeCodeGenerator);
        LineBuffer buf;

        auto result = yebash(suggestion, printer, buf, printBuffer, arrowHandler, c);

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
    EscapeCodeGenerator escapeCodeGenerator;
    Printer printer(output, escapeCodeGenerator);
    ArrowHandler arrowHandler(escapeCodeGenerator);
    LineBuffer buf;
    PrintBuffer printBuffer;

    auto character = 'a';
    auto result = yebash(suggestion, printer, buf, printBuffer, arrowHandler, character);

    REQUIRE(result == character);
    REQUIRE(output.str() == "bc2");

    tearDown();
}

unsigned char rollSuggestions(HistorySuggestion &suggestion, Printer &printer, LineBuffer &buf, PrintBuffer &printBuffer, ArrowHandler &arrowHandler, int n) {
    unsigned char result;
    for (int i = 0; i < n; i++) {
        result = yebash(suggestion, printer, buf, printBuffer, arrowHandler, 0x06);
    }
    return result;
}

TEST_CASE( "Suggestions can be switched", "[basic.browsing_suggestions]" ) {

    History history = createHistory({"a", "ab", "abc", "abcd", "bcd"});
    HistorySuggestion suggestion(history);

    std::stringstream output;
    EscapeCodeGenerator escapeCodeGenerator;
    Printer printer(output, escapeCodeGenerator);
    ArrowHandler arrowHandler(escapeCodeGenerator);
    LineBuffer buf;
    PrintBuffer printBuffer;

    yebash(suggestion, printer, buf, printBuffer, arrowHandler, 'a');

    SECTION( "one switch" ) {
        auto result = rollSuggestions(suggestion, printer, buf, printBuffer, arrowHandler, 1);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbc");
    }

    SECTION( "two switches" ) {
        auto result = rollSuggestions(suggestion, printer, buf, printBuffer, arrowHandler, 2);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbcb");
    }

    SECTION( "third switch to empty suggestion" ) {
        auto result = rollSuggestions(suggestion, printer, buf, printBuffer, arrowHandler, 3);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbcb");
    }

    SECTION( "no more suggestions" ) {
        auto result = rollSuggestions(suggestion, printer, buf, printBuffer, arrowHandler, 4);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbcb");
    }

    SECTION( "go back to the first suggestion" ) {
        auto result = rollSuggestions(suggestion, printer, buf, printBuffer, arrowHandler, 5);
        REQUIRE(result == 0);
        REQUIRE(output.str() == "bcdbcbbcd");
    }

    tearDown();
}


TEST_CASE( "Backspace invalidates suggestions", "[basic.backspace]" ) {

    History history = createHistory({"12345", "def", "trolo", "abc", "123"});
    HistorySuggestion suggestion(history);

    std::stringstream output;
    EscapeCodeGenerator escapeCodeGenerator;
    Printer printer(output, escapeCodeGenerator);
    ArrowHandler arrowHandler(escapeCodeGenerator);
    LineBuffer buf;
    PrintBuffer printBuffer;

    constexpr char backspace = 0x7f;

    yebash(suggestion, printer, buf, printBuffer, arrowHandler, '1');
    yebash(suggestion, printer, buf, printBuffer, arrowHandler, '2');
    yebash(suggestion, printer, buf, printBuffer, arrowHandler, '3');
    yebash(suggestion, printer, buf, printBuffer, arrowHandler, '4');
    REQUIRE(output.str() == "2335");

    yebash(suggestion, printer, buf, printBuffer, arrowHandler, backspace);
    REQUIRE(output.str() == "2335");

    yebash(suggestion, printer, buf, printBuffer, arrowHandler, '4');
    REQUIRE(output.str() == "23355");

    tearDown();
}


TEST_CASE( "Backspaces can't break yebash", "[basic.backspace_underflow]" ) {

    History history = createHistory({"12345", "xyz"});
    HistorySuggestion suggestion(history);

    std::stringstream output;
    EscapeCodeGenerator escapeCodeGenerator;
    Printer printer(output, escapeCodeGenerator);
    ArrowHandler arrowHandler(escapeCodeGenerator);
    LineBuffer buf;
    PrintBuffer printBuffer;

    constexpr char backspace = 0x7f;

    yebash(suggestion, printer, buf, printBuffer, arrowHandler, '1');
    REQUIRE(output.str() == "2345");

    for (int i = 0; i < 1 << 10; ++i) {
        yebash(suggestion, printer, buf, printBuffer, arrowHandler, backspace);
    }
    REQUIRE(output.str() == "2345");

    tearDown();
}

TEST_CASE( "accepts right arrow", "basic.rightArrow" ) {
    History history = createHistory({"abc", "abcd"});
    HistorySuggestion suggestion(history);

    std::stringstream output;
    EscapeCodeGenerator escapeCodeGenerator;
    ANSIEscapeCodeGenerator ansiEscapeCodeGenerator;
    Printer printer(output, escapeCodeGenerator);
    ArrowHandler arrowHandler(ansiEscapeCodeGenerator);
    LineBuffer buf;
    PrintBuffer printBuffer;

    yebash(suggestion, printer, buf, printBuffer, arrowHandler, 'a');
    REQUIRE(output.str() == "bcd");
    REQUIRE(printBuffer == "");
    yebash(suggestion, printer, buf, printBuffer, arrowHandler, '\e');
    yebash(suggestion, printer, buf, printBuffer, arrowHandler, '[');
    yebash(suggestion, printer, buf, printBuffer, arrowHandler, 'C');
    REQUIRE(output.str() == "bcd");
    REQUIRE(printBuffer == "bcd");
}

