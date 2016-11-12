#pragma once

#include "../../src/History.hpp"
#include <initializer_list>
#include <string>

namespace Helpers {

yb::History createHistory(std::initializer_list<std::string> const &commands);

} // namespace Helpers

