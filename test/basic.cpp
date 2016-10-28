#include "yebash.hpp"
#include "History.hpp"

#include "catch.hpp"
#include <sstream>

using namespace yb;

TEST_CASE( "No suggestions when history is empty", "[basic.empty_history]"  ) {
    std::stringstream ss;
    std::stringstream output;
    History history;
    history.read(ss);
    HistorySuggestion suggestion(history);
    Printer printer(output);

    auto c = yebash(suggestion, printer, 'a');
    std::string result{output.str()};

    REQUIRE(c == 'a');
    REQUIRE(output.str() == "");
}

