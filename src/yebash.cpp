#include <functional>
#include <unordered_map>
#include <memory>

#include "yebash.hpp"
#include "HistorySuggestion.hpp"
#include "Defs.hpp"
#include "Printer.hpp"
#include "ArrowHandler.hpp"
#include "LineBuffer.hpp"
#include "KeyHandlers.hpp"
#include "PrintBuffer.hpp"

// https://www.akkadia.org/drepper/tls.pdf
// http://www.tldp.org/HOWTO/Bash-Prompt-HOWTO/x361.html
// http://www.linusakesson.net/programming/tty/

using namespace yb;

constexpr const Color defaultSuggestionColor = Color::grey;
thread_local ColorOpt suggestionColor = {};

thread_local std::unordered_map<Char, std::function<CharOpt(HistorySuggestion &, Printer &, LineBuffer &, Char)>> handlers = {
    {0x06, nextSuggestionHandler},
    {0x0d, newlineHandler},
    {0x17, newlineHandler}, // TODO: this should delete one word
    {0x7f, backspaceHandler}
};

void printSuggestion(HistorySuggestion &history, Printer &printer, LineBuffer &buffer, int offset) {
    std::string pattern{buffer.get()};
    auto suggestion = offset ? history.findSuggestion(pattern) : history.findNextSuggestion(pattern);
    if (!suggestion) {
        printer.clearTerminalLine();
        return;
    }
    if (pattern.length() == suggestion.value().length()) {
        return;
    }
    printer.print(suggestion.value().c_str() + pattern.length(), suggestionColor.value_or(defaultSuggestionColor), offset);
}

namespace yb {

CharOpt newlineHandler(HistorySuggestion &, Printer &, LineBuffer &buffer, Char) {
    buffer.clear();
    return {};
}

CharOpt backspaceHandler(HistorySuggestion &, Printer &, LineBuffer &buffer, Char) {
    buffer.remove();
    return {};
}

CharOpt regularCharHandler(HistorySuggestion &history, Printer &printer, LineBuffer &buffer, Char c) {
    buffer.insert(c);
    printSuggestion(history, printer, buffer, 1);
    return {};
}

CharOpt nextSuggestionHandler(HistorySuggestion &history, Printer &printer, LineBuffer &buffer, Char) {
    printSuggestion(history, printer, buffer, 0);
    return Char{0}; // TODO: this does not seem to work.
}

unsigned char yebash(HistorySuggestion &history, Printer &printer, LineBuffer &buffer, PrintBuffer &printBuffer, ArrowHandler &arrowHandler, unsigned char c) {
    // TODO: uncomment later
    //if (!getenv("YEBASH"))
    //    return;
    auto arrow = arrowHandler.handle(c);
    if (arrow) {
        switch (arrow.value()) {
            case Arrow::right:
                printBuffer = history.get().substr(buffer.getPosition());
                return c;
            case Arrow::left:
                buffer.move(-1);
                return c;
            default:
                return c;
        }
    }
    auto handler = handlers[c];
    CharOpt cReturned;
    if (handler) {
        cReturned = handler(history, printer, buffer, c);
    }
    else {
        if (c < 0x20) {
            newlineHandler(history, printer, buffer, c);
        }
        else {
            regularCharHandler(history, printer, buffer, c);
        }
    }
    return cReturned.value_or(c);
}

} // namespace yb

