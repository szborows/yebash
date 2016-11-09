#pragma once

#include "Defs.hpp"

#include <string>

namespace yb {

class PrintBuffer {

    std::string buffer_;
    std::string::iterator position_;

public:

    PrintBuffer &operator =(std::string &s) {
        buffer_ = s;
        position_ = buffer_.begin();
        return *this;
    }

    CharOpt getNextChar() {
        if (position_ != buffer_.end()) {
            return *position_++;
        }
        return {};
    }

};

} // namespace yb

