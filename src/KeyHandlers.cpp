#include "KeyHandlers.hpp"

extern thread_local char arrowIndicator;

namespace yb {

CharOpt arrowHandler1(History const&, History::const_iterator &, Char) {
    arrowIndicator = 1;
    return {};
}

} // namespace yb

