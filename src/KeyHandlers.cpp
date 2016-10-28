#include "KeyHandlers.hpp"

extern thread_local char arrowIndicator;

namespace yb {

CharOpt escapeHandler(HistorySuggestion &, Printer &, Char) {
    arrowIndicator = 1;
    return {};
}

} // namespace yb

