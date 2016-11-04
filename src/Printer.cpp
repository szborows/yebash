#include "Printer.hpp"
#include <cstring>
#include <iostream>

namespace yb {

void Printer::clearTerminalLine() {
    output_ << escapeCodeHandler_.clearTerminalLine();
}

void Printer::print(const char *text, Color color, int offset) {
    output_ << escapeCodeHandler_.clearTerminalLine()  << escapeCodeHandler_.cursorForward(offset)
            << escapeCodeHandler_.setColor(color) << text << escapeCodeHandler_.setColor(Color::reset)
            << escapeCodeHandler_.cursorBackward(std::strlen(text) + offset) << std::flush;
}

} // namespace yb

