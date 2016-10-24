#include "yebash.hpp"
#include "History.hpp"

#include "catch.hpp"
#include <sstream>

using namespace yb;

TEST_CASE( "No suggestions when history is empty", "[basic.empty_history]"  ) {
    std::stringstream ss;
    History history;
    history.read(ss);
    History::const_iterator historyPos = history.begin();

    auto c = yebash(history, historyPos, 'a');
    REQUIRE(c == 'a');
}
