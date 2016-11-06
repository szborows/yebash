#pragma once

#include "Defs.hpp"

#include <string>

namespace yb {

class LineBuffer {

    using Container = std::string;
    using Iterator = Container::iterator;
    Container buffer_;
    Iterator position_;

public:

    LineBuffer() {
        buffer_.reserve(1024);
        position_ = buffer_.begin();
    }

    void clear() {
        buffer_.clear();
        position_ = buffer_.begin();
    }

    const char *get() const {
        return buffer_.c_str();
    }

    void insert(unsigned char c) {
        buffer_.insert(position_++, c);
    }

    void remove() {
        if (position_ == buffer_.begin()) return;
        buffer_.erase(position_ - 1, position_);
        position_--;
    }

    void move(int n) {
        position_ += n;
    }

};

} // namespace yb

