#pragma once

#include "History.hpp"

namespace yb {

class HistorySuggestion final {

    StringOpt find(const std::string &pattern, History::const_iterator start);
    const History &history_;
    History::const_iterator position_;

public:

    explicit HistorySuggestion(const History &history);
    StringOpt findSuggestion(const std::string &pattern);
    StringOpt findNextSuggestion(const std::string &pattern);
    const auto &get() const  { return *position_; }

};

} // namespace yb

