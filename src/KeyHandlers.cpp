#include "KeyHandlers.hpp"

extern thread_local char arrowIndicator;

namespace yb {

CharOpt arrowHandler1(History const&, Char) {
    arrowIndicator = 1;
    return {};
}

} // namespace yb

