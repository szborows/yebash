#pragma once

#include "Defs.hpp"

#include <array>
#include <string>

namespace yb {

class LineBuffer final {
    static constexpr size_t L = 1024;

    using Container = std::string;
    using Iterator = Container::iterator;
    Container buffer_;
    Iterator position_;
    std::array<unsigned, L> lengths_;

public:

    explicit LineBuffer(size_t size=L);
    void clear();
    const char *get() const;
    void insert(unsigned char c);
    void remove();
    void move(int n);
    size_t getPosition() const;
    char getChar() const;
    bool empty() const;
    void setCharLength(unsigned len);

};

} // namespace yb

