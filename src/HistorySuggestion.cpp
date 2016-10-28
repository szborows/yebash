#include "HistorySuggestion.hpp"

namespace yb {

StringOpt HistorySuggestion::find(const std::string &pattern, History::const_iterator start) {
    for (auto it = start; it != history_.end(); ++it) {
        if (it->compare(0, pattern.length(), pattern) == 0) {
            if (*position_ == *it) continue; // TODO: remove the last whitespace
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

