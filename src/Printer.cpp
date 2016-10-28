#include "Printer.hpp"
#include "TerminalInfo.hpp"
#include <cstring>

namespace yb {

void Printer::deleteRows(int rows) {
    output_ << std::string(rows, ' ');
    cursor_backward(rows);
    output_ << std::flush;
}

void Printer::clearTerminalLine() {
    int pos, width;
    if (!(pos = TerminalInfo::getCursorPosition())) return;
    width = TerminalInfo::getWidth();
    deleteRows(width - pos);
}

void Printer::printInColor(const char *buffer, Color color) {
    output_ << "\e[" << static_cast<int>(color) << 'm' << buffer << "\e[0m";
}

void Printer::print(const char *text, Color color, int offset) {
    clearTerminalLine();
    if (offset)
        cursor_forward(offset);
    printInColor(text, color);
    cursor_backward(std::strlen(text) + offset);
    output_ << std::flush;
}

} // namespace yb

