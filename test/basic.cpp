#include "yebash.hpp"
#include "History.hpp"

#include "catch.hpp"
#include <sstream>

using namespace yb;

TEST_CASE( "No suggestions when history is empty", "[basic.empty_history]"  ) {
    std::stringstream ss;
    History history;
    history.read(ss);
    HistorySuggestion suggestion(history);

    auto c = yebash(suggestion, 'a');
    REQUIRE(c == 'a');
}
