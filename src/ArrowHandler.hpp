#pragma once

#include "Defs.hpp"
#include "Printer.hpp"
#include <functional>
#include <experimental/optional>
#include <unordered_map>
#include <algorithm>

namespace yb {

enum class Arrow {
    left,
    up,
    right,
    down,
    invalid
};

class ArrowHandler final {

    const EscapeCodeGenerator &escapeCodeGenerator_;
    std::unordered_map<Arrow, std::string> escapeCodes_;
    std::string currentState_;

public:

    using ArrowOpt = std::experimental::optional<Arrow>;
    explicit ArrowHandler(const EscapeCodeGenerator &escapeCodeGenerator);
    ArrowOpt handle(unsigned char c);

};

} // namespace yb

