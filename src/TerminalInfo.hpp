#pragma once

namespace yb {

struct TerminalInfo {
    static int getCursorPosition();
    static int getWidth();
};

} // namespace yb
