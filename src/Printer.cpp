#include "Printer.hpp"
#include <cstring>
#include <iostream>

// TODO: change "if (&output_ == &std::cout)" to something more convenient or move it out
// TODO: create class (?) for getting escape codes, which would be passed in constructor

namespace yb {

void Printer::clearTerminalLine() {
    output_ << escapeCodeHandler_.clearTerminalLine();
}

void Printer::printInColor(const char *buffer, Color color) {
    output_ << escapeCodeHandler_.setColor(color) << buffer << escapeCodeHandler_.setColor(Color::reset);
}

void Printer::print(const char *text, Color color, int offset) {
    output_ << escapeCodeHandler_.clearTerminalLine()  << escapeCodeHandler_.cursorForward(offset);
    printInColor(text, color);
    output_ << escapeCodeHandler_.cursorBackward(std::strlen(text) + offset) << std::flush;
}

} // namespace yb

