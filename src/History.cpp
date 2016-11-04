#include "History.hpp"
#include <boost/algorithm/string.hpp>

using namespace std;

namespace yb {

void History::read(istream & input) {
    string line;
    while (getline(input, line)) {
        boost::trim_right(line);
        entries_.push_front(line);
    }
}

} // namespace yb
