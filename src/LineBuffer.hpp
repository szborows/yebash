#pragma once

#include "Defs.hpp"

#include <string>

namespace yb {

class LineBuffer final {

    using Container = std::string;
    using Iterator = Container::iterator;
    Container buffer_;
    Iterator position_;

public:

    explicit LineBuffer(size_t size = 1024);
    void clear();
    const char *get() const;
    void insert(unsigned char c);
    void remove();
    void move(int n);
    size_t getPosition() const;
    char getChar() const;
    bool empty() const;

};

} // namespace yb

