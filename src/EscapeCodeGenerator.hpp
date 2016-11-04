#pragma once

#include "Defs.hpp"
#include <string>

struct EscapeCodeGenerator {

    using EscapeCode = std::string;

    virtual EscapeCode cursorForward(int) const { return EscapeCode{}; }
    virtual EscapeCode cursorBackward(int) const { return EscapeCode{}; }
    virtual EscapeCode clearTerminalLine() const { return EscapeCode{}; }
    virtual EscapeCode setColor(Color) const { return EscapeCode{}; }

};

struct ANSIEscapeCodeGenerator final : EscapeCodeGenerator {

    EscapeCode cursorForward(int n) const override {
        if (!n) return EscapeCode{};
        EscapeCode a{"\033["};
        a += std::to_string(n);
        a += 'C';
        return a;
    }

    EscapeCode cursorBackward(int n) const override {
        if (!n) return EscapeCode{};
        EscapeCode a{"\033["};
        a += std::to_string(n);
        a += 'D';
        return a;
    }

    EscapeCode clearTerminalLine() const override {
        return EscapeCode{"\033[K"};
    }

    EscapeCode setColor(Color color) const override {
        EscapeCode a{"\033["};
        a += std::to_string(static_cast<int>(color));
        a += 'm';
        return a;
    }

};

