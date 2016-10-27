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
    position_ = entries_.begin();
}

StringOpt History::find(const std::string &pattern, const_iterator start) {
    for (auto it = start; it != entries_.end(); ++it) {
        if (it->compare(0, pattern.length(), pattern) == 0) {
            position_ = it;
            return *it;
        }
    }
    position_ = entries_.begin();
    return {};
}

StringOpt History::findCompletion(const std::string &pattern) {
    return find(pattern, entries_.begin());
}

StringOpt History::findNextCompletion(const std::string &pattern) {
    return find(pattern, std::next(position_, 1));
}

} // namespace yb
