#pragma once

#include "Defs.hpp"

#include <string>

namespace yb {

class PrintBuffer final {

    using Container = std::string;
    using Iterator = std::string::const_iterator;
    Container buffer_;
    Iterator position_;

public:

    PrintBuffer() {
        buffer_.reserve(1024);
        position_ = buffer_.end();
    }

    PrintBuffer &operator =(std::string &&s) {
        buffer_ = s;
        position_ = buffer_.begin();
        return *this;
    }

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

    bool empty() const {
        return position_ == buffer_.end();
    }

    bool operator==(const char *a) const {
        return buffer_ == a;
    }

};

} // namespace yb

