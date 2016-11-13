#include "Helpers.hpp"

#include <sstream>

namespace Helpers {

yb::History createHistory(std::initializer_list<std::string> const &commands) {
    std::stringstream ss;
    for (auto && command: commands) {
        ss << command << std::endl;
    }
    yb::History history;
    history.read(ss);
    return history;
}

} // namespace Helpers

