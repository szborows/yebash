#include "../src/History.hpp"
#include "catch.hpp"

#include <sstream>
#include <vector>

using namespace yb;

TEST_CASE( "can read commands from input", "History.read" ) {
    std::vector<std::string> inputList{"", "test", "test2", "test3", "abcd", "a", "dcba", "", "test4", ""};
    std::stringstream inputStream;
    for (auto &&a : inputList) {
        inputStream << a << std::endl;
    }
    History history;
    history.read(inputStream);
    for (std::pair<std::vector<std::string>::iterator, History::const_iterator> it(inputList.end() - 1, history.begin());
            it.second != history.end();
            --it.first, ++it.second) {
        REQUIRE(*it.first == *it.second);
    }
}

