#include "Printer.hpp"
#include <cstring>
#include <iostream>

namespace yb {

void Printer::clearTerminalLine() {
    output_ << escapeCodeGenerator_.clearTerminalLine();
}

void Printer::print(const char *text, Color color, int offset) {
    output_ << escapeCodeGenerator_.clearTerminalLine()  << escapeCodeGenerator_.cursorForward(offset)
            << escapeCodeGenerator_.setColor(color) << text << escapeCodeGenerator_.setColor(Color::reset)
            << escapeCodeGenerator_.cursorBackward(std::strlen(text) + offset) << std::flush;
}

} // namespace yb

