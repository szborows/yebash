#pragma once

#include "Defs.hpp"
#include "EscapeCodeGenerator.hpp"
#include <iostream>

namespace yb {

struct Printer {

    explicit Printer(std::ostream &output, EscapeCodeGenerator &escapeCodeGenerator) : output_(output), escapeCodeGenerator_(escapeCodeGenerator) {}
    void print(const char *text, Color color, int offset);
    void clearTerminalLine();

private:

    std::ostream &output_;
    EscapeCodeGenerator &escapeCodeGenerator_;

};

} // namespace yb


