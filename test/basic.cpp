#include "yebash.hpp"
#include "History.hpp"

#include "catch.hpp"
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

