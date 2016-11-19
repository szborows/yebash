#include "HistorySuggestion.hpp"

namespace yb {

HistorySuggestion::HistorySuggestion(const History &history)
        : history_(history) {
    position_ = history_.begin();
}

StringOpt HistorySuggestion::find(const std::string &pattern, History::const_iterator start) {
    for (auto it = start; it != history_.end(); ++it) {
        if (it->compare(0, pattern.length(), pattern) == 0) {
            if (*position_ == *it && start != history_.begin()) continue;
            position_ = it;
            return *it;
        }
    }
    position_ = history_.begin();
    return {};
}

StringOpt HistorySuggestion::findSuggestion(const std::string &pattern) {
    return find(pattern, history_.begin());
}

StringOpt HistorySuggestion::findNextSuggestion(const std::string &pattern) {
    return find(pattern, std::next(position_, 1));
}

} // namespace yb

