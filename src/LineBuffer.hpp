#pragma once

#include "Defs.hpp"

#include <string>

namespace yb {

class LineBuffer final {

    using Container = std::string;
    using Iterator = Container::iterator;
    Container buffer_;
    Iterator position_;
    const size_t defaultLineBufferSize = 1024;

public:

    LineBuffer() {
        buffer_.reserve(defaultLineBufferSize);
        position_ = buffer_.begin();
    }

    explicit LineBuffer(size_t size) {
        buffer_.reserve(size);
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

    size_t getPosition() const {
        return position_ - buffer_.begin();
    }

    char getChar() const {
        return *position_;
    }

};

} // namespace yb

