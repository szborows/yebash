#pragma once

#include "Defs.hpp"
#include "Printer.hpp"
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

class ArrowHandler final {

    const EscapeCodeGenerator &escapeCodeGenerator_;
    std::unordered_map<Arrow, std::string> escapeCodes_;
    std::string currentState_;

public:

    using ArrowOpt = std::experimental::optional<Arrow>;

    explicit ArrowHandler(const EscapeCodeGenerator &escapeCodeGenerator) : escapeCodeGenerator_(escapeCodeGenerator) {
        escapeCodes_[Arrow::left] = escapeCodeGenerator_.cursorBackward(1);
        escapeCodes_[Arrow::up] = escapeCodeGenerator_.cursorUp(1);
        escapeCodes_[Arrow::right] = escapeCodeGenerator_.cursorForward(1);
        escapeCodes_[Arrow::down] = escapeCodeGenerator_.cursorDown(1);
        currentState_.reserve(5);
    } 

    inline ArrowOpt handle(unsigned char c) {
        currentState_ += c;
        for (auto it = escapeCodes_.begin(); it != escapeCodes_.end(); ++it) {
            auto arrow = std::get<0>(*it);
            auto ec = std::get<1>(*it);
            if (ec.compare(0, currentState_.length(), currentState_) == 0) {
                if (ec.length() == currentState_.length()) {
                    currentState_.clear();
                    return arrow;
                }
                return Arrow::no;
            }
        }
        currentState_.clear();
        return {};
    }

};

} // namespace yb

