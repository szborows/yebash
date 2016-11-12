#include "../src/HistorySuggestion.hpp"
#include "helpers/Helpers.hpp"
#include "catch.hpp"

using namespace yb;

namespace {

void testSuggestions(HistorySuggestion &suggestion, std::string chars, std::initializer_list<std::string> expected) {
    for (std::pair<std::string::const_iterator, std::initializer_list<std::string>::const_iterator> it{chars.begin(), expected.begin()};
            it.first != chars.end(); ++it.first, ++it.second) {
        auto result = suggestion.findSuggestion(std::string{*it.first});
        REQUIRE(result.value_or("") == *it.second);
    }
}

} // namespace anon

TEST_CASE( "can get position", "[HistorySuggestion.position]" ) {
    auto history = Helpers::createHistory({"abcd", "bcde", "cdef", "defg", "efgh", "fghi"});
    HistorySuggestion suggestion(history);
    REQUIRE(suggestion.get() == *history.begin());
}

TEST_CASE( "can find suggestion", "[HistorySuggestion.findSuggestion]" ) {
    std::initializer_list<std::string> historyEntries{"abcd", "bcde", "cdef", "defg", "efgh", "fghi"};
    auto history = Helpers::createHistory(historyEntries);
    HistorySuggestion suggestion(history);
    testSuggestions(suggestion, "abcdef", historyEntries);
}

