#pragma once

namespace yb {

class TerminalInfo {

    int cursorRow;

public:

    int getCursorPosition();
    int getWidth();

};

} // namespace yb
