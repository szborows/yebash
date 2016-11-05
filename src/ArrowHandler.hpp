#pragma once

#include "Defs.hpp"
#include "HistorySuggestion.hpp"
#include "Printer.hpp"
#include <array>
#include <functional>
#include <experimental/optional>

namespace yb {

enum class Arrow {
    left,
    up,
    right,
    down
};

class ArrowHandler {

    const int left_ = 0;
    const int up_ = 1;
    const int right_ = 2;
    const int down_ = 3;

    using Handler = void(HistorySuggestion &, Printer &);
    const EscapeCodeGenerator &escapeCodeGenerator_;
    std::array<std::string, 4> escapeCodes = {{"\e[1D", "\e[1A", "\e[1C", "\e[1B"}};
    std::string currentState;
    std::string::iterator currentStateIterator;

public:

    using ArrowOpt = std::experimental::optional<Arrow>;

    ArrowHandler(const EscapeCodeGenerator &escapeCodeGenerator) : escapeCodeGenerator_(escapeCodeGenerator) {
        currentState.resize(5);
        currentStateIterator = currentState.begin();
    } 

    ArrowOpt handle(unsigned char c) {
        *currentStateIterator++ = c;
        for (const auto &it : escapeCodes) {
            if (it.compare(0, currentState.length(), currentState)) {
                return {}; // TODO: return proper Arrow value
            }
        }
        currentState.clear();
        currentStateIterator = currentState.begin();
        return {};
    }

};

} // namespace yb

