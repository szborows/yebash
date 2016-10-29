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


