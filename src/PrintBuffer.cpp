#include "PrintBuffer.hpp"

namespace yb {

PrintBuffer::PrintBuffer(size_t size) {
    buffer_.reserve(size);
    position_ = buffer_.end();
}

PrintBuffer &PrintBuffer::operator =(std::string &&s) {
    buffer_ = s;
    position_ = buffer_.begin();
    return *this;
}

PrintBuffer &PrintBuffer::operator =(std::string &s) {
    buffer_ = s;
    position_ = buffer_.begin();
    return *this;
}

CharOpt PrintBuffer::getNextChar() {
    if (position_ != buffer_.end()) {
        return *position_++;
    }
    return {};
}

bool PrintBuffer::empty() const {
    return position_ == buffer_.end();
}

bool PrintBuffer::operator==(const char *a) const {
    return buffer_ == a;
}

} // namespace yb
