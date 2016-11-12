#include "../src/History.hpp"
#include "helpers/Helpers.hpp"
#include "catch.hpp"

#include <initializer_list>

using namespace yb;

TEST_CASE( "can read commands from input", "History.read" ) {
    std::initializer_list<std::string> inputList{"", "test", "test2", "test3", "abcd", "a", "dcba", "", "test4", ""};
    auto history = Helpers::createHistory(inputList);
    size_t i = 0;
    for (std::pair<std::initializer_list<std::string>::iterator, History::const_iterator> it(inputList.end() - 1, history.begin());
            it.second != history.end();
            --it.first, ++it.second) {
        REQUIRE(*it.first == *it.second);
        i++;
    }
    REQUIRE(i == inputList.size());
}

