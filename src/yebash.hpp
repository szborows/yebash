#pragma once

#include "HistorySuggestion.hpp"
#include "Printer.hpp"
#include "LineBuffer.hpp"

namespace yb {

unsigned char yebash(HistorySuggestion &history, Printer &printer, LineBuffer &buffer, unsigned char c);

} // namespace yb

