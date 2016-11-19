#include "ArrowHandler.hpp"

namespace yb {

ArrowHandler::ArrowHandler(const EscapeCodeGenerator &escapeCodeGenerator) : escapeCodeGenerator_(escapeCodeGenerator) {
    escapeCodes_[Arrow::left] = escapeCodeGenerator_.cursorBackward(1);
    escapeCodes_[Arrow::up] = escapeCodeGenerator_.cursorUp(1);
    escapeCodes_[Arrow::right] = escapeCodeGenerator_.cursorForward(1);
    escapeCodes_[Arrow::down] = escapeCodeGenerator_.cursorDown(1);
    currentState_.reserve(std::get<1>(*std::max_element(escapeCodes_.begin(), escapeCodes_.end(),
                [] (const std::unordered_map<Arrow, std::string>::value_type &A, const std::unordered_map<Arrow, std::string>::value_type &B) {
                    return A.second.length() > B.second.length();
                })).length() + 1);
}

ArrowHandler::ArrowOpt ArrowHandler::handle(unsigned char c) {
    currentState_ += c;
    for (auto it = escapeCodes_.begin(); it != escapeCodes_.end(); ++it) {
        auto arrow = std::get<0>(*it);
        auto ec = std::get<1>(*it);
        if (ec.compare(0, currentState_.length(), currentState_) == 0) {
            if (ec.length() == currentState_.length()) {
                currentState_.clear();
                return arrow;
            }
            return Arrow::invalid;
        }
    }
    currentState_.clear();
    return {};
}

} // namespace yb
