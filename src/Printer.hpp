#pragma once

#include "Defs.hpp"
#include "EscapeCodeGenerator.hpp"
#include <iostream>

namespace yb {

class Printer final {

    std::ostream &output_;
    const EscapeCodeGenerator &escapeCodeGenerator_;

public:

    explicit Printer(std::ostream &output, const EscapeCodeGenerator &escapeCodeGenerator) : output_(output), escapeCodeGenerator_(escapeCodeGenerator) {}
    void print(const char *text, Color color, int offset);
    void clearTerminalLine();

};

} // namespace yb

