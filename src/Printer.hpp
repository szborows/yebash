#pragma once

#include "Defs.hpp"
#include "EscapeCodeGenerator.hpp"
#include <iostream>

namespace yb {

struct Printer final {

    explicit Printer(std::ostream &output, const EscapeCodeGenerator &escapeCodeGenerator) : output_(output), escapeCodeGenerator_(escapeCodeGenerator) {}
    void print(const char *text, Color color, int offset);
    void clearTerminalLine();

private:

    std::ostream &output_;
    const EscapeCodeGenerator &escapeCodeGenerator_;

};

} // namespace yb


