#include "UnicodeHandler.hpp"

namespace yb {

unsigned UnicodeHandler::handle(unsigned char c) {
    if (left_) {
        --left_;
        if (left_) {
            return 0;
        }
    }
    else if ((c >> 3) == 0x1e) {
        left_ = 3;
        return 4;
    }
    else if ((c >> 4) == 0xe) {
        left_ = 2;
        return 3;
    }
    else if (c >= 0x80) {
        left_ = 1;
        return 2;
    }

    return 1;
}

} // namespace yb
