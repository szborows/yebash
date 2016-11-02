#include "Printer.hpp"
#include <cstring>
#include <iostream>

// TODO: change "if (&output_ == &std::cout)" to something more convenient or move it out
// TODO: create class (?) for getting escape codes, which would be passed in constructor

namespace yb {

void Printer::clearTerminalLine() {
    if (&output_ == &std::cout) {
       output_ << "\033[K" << std::flush;
    }
}

void Printer::printInColor(const char *buffer, Color color) {
    if (&output_ == &std::cout)
        output_ << "\e[" << static_cast<int>(color) << 'm';
    output_ << buffer;
    if (&output_ == &std::cout)
        output_ << "\e[0m";
}

void Printer::print(const char *text, Color color, int offset) {
    if (&output_ == &std::cout) {
        clearTerminalLine();
        if (offset)
           cursor_forward(offset);
    }
    printInColor(text, color);
    if (&output_ == &std::cout) {
        cursor_backward(std::strlen(text) + offset);
    }
    output_ << std::flush;

}

} // namespace yb

