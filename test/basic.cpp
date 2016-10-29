#include "yebash.hpp"
#include "History.hpp"

#include "catch.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace yb;
using namespace std;

TEST_CASE( "No suggestions when history is empty", "[basic.empty_history]"  ) {

    auto testCharacter = [] (char const c) {
        std::stringstream ss;
        std::stringstream output;
        History history;
        history.read(ss);
        HistorySuggestion suggestion(history);
        Printer printer(output);

        auto result = yebash(suggestion, printer, c);

        REQUIRE(result == c);
        REQUIRE(output.str() == "");
    };

    string domain = "abcdefghijklmnopqrstuvwxyz01234567890-_";
    for_each(begin(domain), end(domain), testCharacter);
}


TEST_CASE( "Order of commands from history is preserved", "[basic.history_order_preserved]"  ) {

    std::stringstream ss;
    ss << "abc1" << std::endl;
    ss << "abc2" << std::endl;

    std::stringstream output;
    History history;
    history.read(ss);
    HistorySuggestion suggestion(history);
    Printer printer(output);

    auto character = 'a';
    auto result = yebash(suggestion, printer, character);

    REQUIRE(result == character);
    REQUIRE(output.str() == "abc2");
}


