#pragma once

#include "Defs.hpp"
#include "TerminalInfo.hpp"
#include <iostream>

namespace yb {

struct Printer {

    explicit Printer(std::ostream &output, TerminalInfo &terminalInfo) : output_(output), terminalInfo_(terminalInfo) {}
    void print(const char *text, Color color, int offset);
    void clearTerminalLine();

private:

    void printInColor(const char *buffer, Color color);
    void deleteRows(int rows);

    inline void cursor_forward(int x) {
        output_ << "\033[" << x << 'C';
    }

    inline void cursor_backward(int x) {
        output_ << "\033[" << x << 'D';
    }

    std::ostream &output_;
    TerminalInfo &terminalInfo_;

};

} // namespace yb


