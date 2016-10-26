#pragma once

#include "History.hpp"

namespace yb {

// TODO: move here findCompletion method and history position (from the History class)
struct HistorySuggestion {

    HistorySuggestion(const History &history) : history_(history) {};

private:
    const History &history_;

};

} // namespace yb

