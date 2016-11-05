#pragma once

#include "Defs.hpp"
#include <string>

namespace yb {

struct EscapeCodeGenerator {

    using EscapeCode = std::string;

    virtual EscapeCode cursorForward(unsigned int) const { return EscapeCode{}; }
    virtual EscapeCode cursorBackward(unsigned int) const { return EscapeCode{}; }
    virtual EscapeCode clearTerminalLine() const { return EscapeCode{}; }
    virtual EscapeCode setColor(Color) const { return EscapeCode{}; }
    virtual ~EscapeCodeGenerator() {}

};

// https://en.wikipedia.org/wiki/ANSI_escape_code
struct ANSIEscapeCodeGenerator final : EscapeCodeGenerator {

    EscapeCode cursorForward(unsigned int n) const override {
        if (!n) return EscapeCode{};
        return EscapeCodeStart + std::to_string(n) + 'C';
    }

    EscapeCode cursorBackward(unsigned int n) const override {
        if (!n) return EscapeCode{};
        return EscapeCodeStart + std::to_string(n) + 'D';
    }

    EscapeCode clearTerminalLine() const override {
        return EscapeCodeStart + 'K';
    }

    EscapeCode setColor(Color color) const override {
        return EscapeCodeStart + std::to_string(static_cast<int>(color)) + 'm';
    }

    ~ANSIEscapeCodeGenerator() override {}

private:
    const EscapeCode EscapeCodeStart = "\033[";

};

} // namespace yb

