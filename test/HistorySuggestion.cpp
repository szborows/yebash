#include "../src/HistorySuggestion.hpp"
#include "helpers/Helpers.hpp"
#include "catch.hpp"

using namespace yb;

TEST_CASE( "can get position", "[HistorySuggestion.position]" ) {
    auto history = Helpers::createHistory({"abcd", "bcde", "cdef", "defg", "efgh", "fghi"});
    HistorySuggestion suggestion(history);
    REQUIRE(suggestion.get() == *history.begin());
}

