#pragma once

#include "History.hpp"

namespace yb {

// TODO: move here findCompletion method and history position (from the History class)
struct HistorySuggestion {

    HistorySuggestion(const History &history) : history_(history) { position_ = history_.begin(); }

    StringOpt findCompletion(const std::string &pattern);
    StringOpt findNextCompletion(const std::string &pattern);

    auto &get() { return *position_; }

private:
    StringOpt find(const std::string &pattern, History::const_iterator start);
    const History &history_;
    History::const_iterator position_;
};

} // namespace yb

