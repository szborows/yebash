#include "Printer.hpp"
#include "TerminalInfo.hpp"
#include <cstring>

namespace yb {

static inline void cursor_forward(int x) {
    printf("\033[%dC", x);
}

static inline void cursor_backward(int x) {
    printf("\033[%dD", x);
}

static inline void deleteRows(int rows) {
    std::cout << std::string(rows, ' ');
    cursor_backward(rows);
    std::cout << std::flush;
}

static inline void clearTerminalLine() {
    int pos, width;
    if (!(pos = TerminalInfo::getCursorPosition())) return;
    width = TerminalInfo::getWidth();
    deleteRows(width - pos);
}

static inline void printColor(const char *buffer, Color color) {
    std::cout << "\e[" << static_cast<int>(color) << 'm' << buffer << "\e[0m";
}

void Printer::print(const char *text, Color color, int offset) {
    clearTerminalLine();
    if (offset)
        cursor_forward(offset);
    printColor(text, color);
    cursor_backward(std::strlen(text) + offset);
    std::cout << std::flush;
}

} // namespace yb

