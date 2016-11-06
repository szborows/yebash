#pragma once

#include "Defs.hpp"
#include "HistorySuggestion.hpp"
#include "Printer.hpp"
#include <array>
#include <functional>
#include <experimental/optional>
#include <unordered_map>

namespace yb {

enum class Arrow {
    no,
    left,
    up,
    right,
    down
};

class ArrowHandler {

    using Handler = void(HistorySuggestion &, Printer &);
    const EscapeCodeGenerator &escapeCodeGenerator_;
    std::unordered_map<Arrow, std::string> escapeCodes_;
    std::string currentState;
    std::string::iterator currentStateIterator;

public:

    using ArrowOpt = std::experimental::optional<Arrow>;

    explicit ArrowHandler(const EscapeCodeGenerator &escapeCodeGenerator) : escapeCodeGenerator_(escapeCodeGenerator) {
        escapeCodes_[Arrow::left] = escapeCodeGenerator_.cursorBackward(1);
        escapeCodes_[Arrow::up] = escapeCodeGenerator_.cursorUp(1);
        escapeCodes_[Arrow::right] = escapeCodeGenerator_.cursorForward(1);
        escapeCodes_[Arrow::down] = escapeCodeGenerator_.cursorDown(1);
        currentState.resize(5);
        currentStateIterator = currentState.begin();
    } 

    ArrowOpt handle(unsigned char c) {
        *currentStateIterator++ = c;
        for (auto it = escapeCodes_.begin(); it != escapeCodes_.end(); ++it) {
            auto arrow = std::get<0>(*it);
            auto ec = std::get<1>(*it);
            if (!ec.compare(0, currentState.length(), currentState)) {
                if (ec.length() == currentState.length()) {
                    return arrow;
                }
                return Arrow::no;
            }
        }
        currentState.clear();
        currentStateIterator = currentState.begin();
        return {};
    }

};

} // namespace yb

