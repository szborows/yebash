#pragma once

#include "Defs.hpp"
#include <iostream>

namespace yb {

struct Printer {

    Printer() = delete;
    Printer(std::ostream &output) : output_(output) {}
    void print(const char *text, Color color, int offset);

private:
    std::ostream &output_;

};

} // namespace yb


