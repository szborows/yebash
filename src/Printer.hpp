#pragma once

#include "Defs.hpp"
#include "EscapeCodeHandler.hpp"
#include <iostream>

namespace yb {

struct Printer {

    explicit Printer(std::ostream &output, EscapeCodeHandler &escapeCodeHandler) : output_(output), escapeCodeHandler_(escapeCodeHandler) {}
    void print(const char *text, Color color, int offset);
    void clearTerminalLine();

private:

    std::ostream &output_;
    EscapeCodeHandler &escapeCodeHandler_;

};

} // namespace yb


