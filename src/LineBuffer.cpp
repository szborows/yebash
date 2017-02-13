#include "LineBuffer.hpp"
#include <algorithm>

namespace yb {

LineBuffer::LineBuffer(size_t size) {
    buffer_.reserve(size);
    position_ = buffer_.begin();
}

void LineBuffer::clear() {
    buffer_.clear();
    position_ = buffer_.begin();
}

const char *LineBuffer::get() const {
    return buffer_.c_str();
}

void LineBuffer::insert(unsigned char c) {
    buffer_.insert(position_++, c);
}

void LineBuffer::remove() {
    if (position_ == buffer_.begin()) return;
    buffer_.erase(position_ - 1, position_);
    position_--;
}

void LineBuffer::move(int n) {
    auto begin = std::begin(buffer_);
    if (n < 0 && std::distance(begin, position_) < -n) {
        position_ = begin;
    }
    else {
        position_ += n;
    }
}

size_t LineBuffer::getPosition() const {
    return position_ - buffer_.begin();
}

char LineBuffer::getChar() const {
    return *position_;
}

bool LineBuffer::empty() const {
    return buffer_.empty();
}

void LineBuffer::setCharLength(unsigned len) {
    lengths_[std::distance(std::begin(buffer_        position_] = len;
}

} // namespace yb

