#pragma once

#include "Defs.hpp"
#include <string>

struct EscapeCodeHandler {

    using EscapeCode = std::string;

    virtual EscapeCode cursorForward(int) { return EscapeCode{}; }
    virtual EscapeCode cursorBackward(int) { return EscapeCode{}; }
    virtual EscapeCode clearTerminalLine() { return EscapeCode{}; }
    virtual EscapeCode setColor(Color) { return EscapeCode{}; }

};

struct ANSIEscapeCodeHandler : public EscapeCodeHandler {

    EscapeCode cursorForward(int n) override {
        if (!n) return EscapeCode{};
        EscapeCode a{"\033["};
        a += std::to_string(n);
        a += 'C';
        return a;
    }

    EscapeCode cursorBackward(int n) override {
        if (!n) return EscapeCode{};
        EscapeCode a{"\033["};
        a += std::to_string(n);
        a += 'D';
        return a;
    }

    EscapeCode clearTerminalLine() override {
        return EscapeCode{"\033[K"};
    }

    EscapeCode setColor(Color color) override {
        EscapeCode a{"\033["};
        a += std::to_string(static_cast<int>(color));
        a += 'm';
        return a;
    }

};

