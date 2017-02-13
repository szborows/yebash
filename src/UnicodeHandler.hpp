#pragma once

namespace yb {

class UnicodeHandler final {
    unsigned left_ = 0;

public:
    unsigned handle(unsigned char c);
};

} // namespace yb

