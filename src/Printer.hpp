#pragma once

#include "Defs.hpp"
#include <iostream>

namespace yb {

struct EscapeCodeHandler {

    using EscapeCode = std::string;

    virtual EscapeCode cursorForward(int n);
    virtual EscapeCode cursorBackward(int n);
    virtual EscapeCode clearTerminalLine();
    virtual EscapeCode setColor(Color color);
};

struct ANSIEscapeCodeHandler : EscapeCodeHandler {

    EscapeCode cursorForward(int n) override {
        std::string a{"\e["};
        a += n;
        a += 'C';
        return a;
    }

    EscapeCode cursorBackward(int n) override {
        std::string a{"\e["};
        a += n;
        a += 'D';
        return a;
    }

    EscapeCode clearTerminalLine() override {
        return EscapeCode{"\e[K"};
    }

    EscapeCode setColor(Color color) override {
        EscapeCode a{"\e["};
        a += static_cast<int>(color);
        a += 'm';
        return a;
    }

};

struct Printer {

    explicit Printer(std::ostream &output) : output_(output) {}
    void print(const char *text, Color color, int offset);
    void clearTerminalLine();

private:

    void printInColor(const char *buffer, Color color);

    inline void cursor_forward(int x) {
        output_ << "\033[" << x << 'C';
    }

    inline void cursor_backward(int x) {
        output_ << "\033[" << x << 'D';
    }

    std::ostream &output_;

};

} // namespace yb


