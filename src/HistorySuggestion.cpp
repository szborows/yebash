#include "HistorySuggestion.hpp"

namespace yb {

StringOpt HistorySuggestion::find(const std::string &pattern, History::const_iterator start) {
    for (auto it = start; it != history_.end(); ++it) {
        if (it->compare(0, pattern.length(), pattern) == 0) {
            position_ = it;
            return *it;
        }
    }
    position_ = history_.begin();
    return {};
}

StringOpt HistorySuggestion::findCompletion(const std::string &pattern) {
    return find(pattern, history_.begin());
}

StringOpt HistorySuggestion::findNextCompletion(const std::string &pattern) {
    return find(pattern, std::next(position_, 1));
}

} // namespace yb

