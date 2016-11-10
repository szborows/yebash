#pragma once

#include "Defs.hpp"

#include <string>

namespace yb {

class PrintBuffer final {

    using Container = std::string;
    using Iterator = std::string::const_iterator;
    Container buffer_;
    Iterator position_;
    const size_t defaultBufferSize_ = 1024;

public:

    PrintBuffer() {
        buffer_.reserve(defaultBufferSize_);
        position_ = buffer_.end();
    }

    explicit PrintBuffer(size_t size) {
        buffer_.reserve(size);
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

