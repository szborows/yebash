#include <dlfcn.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <functional>
#include <stdexcept>
#include <memory>
#include <unordered_map>

#include "yebash.hpp"
#include "HistorySuggestion.hpp"
#include "Defs.hpp"
#include "KeyHandlers.hpp"
#include "Printer.hpp"
#include "ArrowHandler.hpp"

// https://www.akkadia.org/drepper/tls.pdf
// http://www.tldp.org/HOWTO/Bash-Prompt-HOWTO/x361.html
// http://www.linusakesson.net/programming/tty/

using namespace yb;

thread_local std::array<char, 1024> lineBuffer;
thread_local auto lineBufferPos = lineBuffer.begin();

thread_local std::string printBuffer;
thread_local std::string::iterator printBufferPos;

thread_local History gHistory;
thread_local History::const_iterator gHistoryPos;

thread_local char arrowIndicator = 0;

using ReadSignature = ssize_t (*)(int, void*, size_t);
static thread_local ReadSignature realRead = nullptr;

constexpr const Color defaultSuggestionColor = Color::grey;
thread_local ColorOpt suggestionColor = {};

CharOpt newlineHandler(HistorySuggestion &, Printer &, Char);
CharOpt tabHandler(HistorySuggestion &, Printer &, Char);
CharOpt backspaceHandler(HistorySuggestion &, Printer &, Char);
CharOpt regularCHarHandler(HistorySuggestion &, Printer &, Char);
CharOpt arrowHandler1(HistorySuggestion &, Printer &, Char);
CharOpt arrowHandler2(HistorySuggestion &, Printer &, Char);
CharOpt arrowHandler3(HistorySuggestion &, Printer &, Char);

thread_local std::unique_ptr<HistorySuggestion> historySuggestion = nullptr;
thread_local std::unique_ptr<EscapeCodeGenerator> escapeCodeGenerator = nullptr;
thread_local std::unique_ptr<Printer> printer = nullptr;

thread_local std::unordered_map<Char, std::function<CharOpt(HistorySuggestion &, Printer &, Char)>> handlers = {
    {0x06, tabHandler},
    {0x0d, newlineHandler},
    {0x17, newlineHandler}, // TODO: this should delete one word
    {0x1b, yb::arrowHandler1},
    {0x5b, arrowHandler2},
    {0x43, arrowHandler3},
    {0x7f, backspaceHandler}
};

void printSuggestion(HistorySuggestion &history, Printer &printer, int offset) {
    std::string pattern(lineBuffer.data());
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

CharOpt newlineHandler(HistorySuggestion &, Printer &, Char) {
    lineBuffer.fill(0);
    lineBufferPos = lineBuffer.begin();
    return {};
}

CharOpt backspaceHandler(HistorySuggestion &, Printer &, Char) {
    if (lineBufferPos != lineBuffer.begin()) {
        *(--lineBufferPos) = 0;
    }
    return {};
}

CharOpt regularCharHandler(HistorySuggestion &history, Printer &printer, Char c) {
    *lineBufferPos = c;
    lineBufferPos++;
    printSuggestion(history, printer, 1);
    return {};
}

CharOpt tabHandler(HistorySuggestion &history, Printer &printer, Char) {
    printSuggestion(history, printer, 0);
    return Char{0}; // TODO: this does not seem to work.
}

CharOpt arrowHandler2(HistorySuggestion &history, Printer &printer, Char c) {
    if (arrowIndicator == 1) {
        arrowIndicator = 2;
        return {};
    }
    else {
        return regularCharHandler(history, printer, c);
    }
}

CharOpt arrowHandler3(HistorySuggestion &history, Printer &printer, Char c) {
    CharOpt return_value = {};
    if (arrowIndicator == 2) {
        arrowIndicator = 0;
        try {
            printBuffer = historySuggestion->get().substr(lineBufferPos - lineBuffer.begin());
            printBufferPos = printBuffer.begin();
        } catch (...) {
            // FIXME:
        }
    }
    else {
        return_value = regularCharHandler(history, printer, c);
    }
    return return_value;
}

namespace yb {

unsigned char yebash(HistorySuggestion &history, Printer &printer, unsigned char c) {
    // TODO: uncomment later
    //if (!getenv("YEBASH"))
    //    return;
    auto handler = handlers[c];
    CharOpt cReturned;
    if (handler) {
        cReturned = handler(history, printer, c);
    }
    else {
        if (c < 0x20) {
            newlineHandler(history, printer, c);
        }
        else {
            regularCharHandler(history, printer, c);
        }
    }
    return cReturned.value_or(c);
}

} // namespace yb

static inline bool is_terminal_input(int fd) {
    return isatty(fd) && fd == 0;
}

static inline void putCharToReadBuffer(char *buf) {
    *buf = *printBufferPos;
    *lineBufferPos++ =  *printBufferPos++;
    if (printBufferPos == printBuffer.end()) {
        printBuffer.erase(printBuffer.begin(), printBuffer.end());
    }
}

ssize_t read(int fd, void *buf, size_t count) {
    if (is_terminal_input(fd) && printBuffer.length()) {
        putCharToReadBuffer(static_cast<char *>(buf));
        return 1;
    }
    auto returnValue = realRead(fd, buf, count);
    if (is_terminal_input(fd)) {
        *static_cast<unsigned char *>(buf) = yb::yebash(*historySuggestion, *printer, *static_cast<unsigned char *>(buf));
    }
    return returnValue;
}

__attribute__((constructor))
static void yebashInit()  {
    if (!realRead) {
        realRead = reinterpret_cast<ReadSignature>(dlsym(RTLD_NEXT, "read"));
    }
    std::ifstream historyFile(std::string{getenv("HOME")} + "/.bash_history");
    if (!historyFile.is_open()) {
        throw std::runtime_error{"Could not open history file"};
    }
    gHistory.read(historyFile);
    historyFile.close();
    historySuggestion = std::make_unique<HistorySuggestion>(gHistory);
    escapeCodeGenerator = std::make_unique<ANSIEscapeCodeGenerator>();
    printer = std::make_unique<Printer>(std::cout, *escapeCodeGenerator);
}

