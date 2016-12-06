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

    explicit PrintBuffer(size_t size = 1024);
    PrintBuffer &operator =(const std::string &s);
    CharOpt getNextChar();
    bool empty() const;
    bool operator==(const char *a) const;

};

} // namespace yb

