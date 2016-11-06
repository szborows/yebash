#pragma once

#include "Defs.hpp"
#include "HistorySuggestion.hpp"
#include "Printer.hpp"
#include "LineBuffer.hpp"

namespace yb {

CharOpt newlineHandler(HistorySuggestion &, Printer &, LineBuffer &, Char);
CharOpt tabHandler(HistorySuggestion &, Printer &, LineBuffer &, Char);
CharOpt backspaceHandler(HistorySuggestion &, Printer &, LineBuffer &, Char);
CharOpt regularCharHandler(HistorySuggestion &, Printer &, LineBuffer &, Char);

} // namespace yb

