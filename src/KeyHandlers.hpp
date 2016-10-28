#pragma once

#include "yebash.hpp"

namespace yb {

CharOpt newlineHandler(HistorySuggestion &, Printer &, Char);
CharOpt tabHandler(HistorySuggestion &, Printer &, Char);
CharOpt backspaceHandler(HistorySuggestion &, Printer &, Char);
CharOpt regularCharHandler(HistorySuggestion &, Printer &, Char);
CharOpt escapeHandler(HistorySuggestion &, Printer &, Char);
CharOpt arrowHandler2(HistorySuggestion &, Printer &, Char);
CharOpt arrowHandler3(HistorySuggestion &, Printer &, Char);

} // namespace yb

