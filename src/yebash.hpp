#pragma once

#include "HistorySuggestion.hpp"
#include "Printer.hpp"
#include "LineBuffer.hpp"
#include "PrintBuffer.hpp"
#include "ArrowHandler.hpp"

namespace yb {

unsigned char yebash(HistorySuggestion &history, Printer &printer, LineBuffer &buffer, PrintBuffer &printBuffer, ArrowHandler &arrowHandler, unsigned char c);

} // namespace yb

