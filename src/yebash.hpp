#pragma once

#include "HistorySuggestion.hpp"
#include "Printer.hpp"

namespace yb {

unsigned char yebash(HistorySuggestion &history, Printer &printer, unsigned char c);

} // namespace yb

