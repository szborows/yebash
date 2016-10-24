#include "History.hpp"

using namespace std;

namespace yb {

void History::read(istream & input) {
    if (!entries_.empty()) {
        return;
    }

    string line;
    while (getline(input, line)) {
        entries_.push_front(line);
    }
}

} // namespace yb
