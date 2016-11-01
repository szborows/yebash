#pragma once

#include "Defs.hpp"
#include <iostream>

namespace yb {

struct Printer {

    explicit Printer(std::ostream &output) : output_(output) {}
    void print(const char *text, Color color, int offset);
    void clearTerminalLine();

private:

    void printInColor(const char *buffer, Color color);

    inline void cursor_forward(int x) {
        output_ << "\033[" << x << 'C';
    }

    inline void cursor_backward(int x) {
        output_ << "\033[" << x << 'D';
    }

    std::ostream &output_;

};

} // namespace yb


