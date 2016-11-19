#include "LineBuffer.hpp"

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
    position_ += n;
}

size_t LineBuffer::getPosition() const {
    return position_ - buffer_.begin();
}

char LineBuffer::getChar() const {
    return *position_;
}

} // namespace yb

