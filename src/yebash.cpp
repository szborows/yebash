#include <dlfcn.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <map>
#include <functional>
#include <stdexcept>
#include <memory>

#include "yebash.hpp"
#include "HistorySuggestion.hpp"
#include "Defs.hpp"
#include "TerminalInfo.hpp"
#include "KeyHandlers.hpp"
#include "Printer.hpp"

#define cursor_forward(x) printf("\033[%dC", static_cast<int>(x))
#define cursor_backward(x) printf("\033[%dD", static_cast<int>(x))

// https://www.akkadia.org/drepper/tls.pdf

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

using ColorOpt = std::experimental::optional<Color>;
constexpr const Color defaultCompletionColor = Color::grey;
thread_local ColorOpt completionColor = {};

CharOpt newlineHandler(HistorySuggestion &, Char);
CharOpt tabHandler(HistorySuggestion &, Char);
CharOpt backspaceHandler(HistorySuggestion &, Char);
CharOpt regularCHarHandler(HistorySuggestion &, Char);
CharOpt arrowHandler1(HistorySuggestion &, Char);
CharOpt arrowHandler2(HistorySuggestion &, Char);
CharOpt arrowHandler3(HistorySuggestion &, Char);

thread_local std::unique_ptr<HistorySuggestion> historySuggestion = nullptr;

thread_local std::map<Char, std::function<CharOpt(HistorySuggestion &, Char)>> handlers = {
    {0x06, tabHandler},
    {0x0d, newlineHandler},
    {0x17, newlineHandler}, // TODO: this should delete one word
    {0x1b, yb::arrowHandler1},
    {0x5b, arrowHandler2},
    {0x43, arrowHandler3},
    {0x7f, backspaceHandler}
};

static inline void deleteRows(int rows) {
    std::cout << std::string(rows, ' ');
    cursor_backward(rows);
    std::cout << std::flush;
}

void clearTerminalLine() {
    int pos, width;
    if (!(pos = TerminalInfo::getCursorPosition())) return;
    width = TerminalInfo::getWidth();
    deleteRows(width - pos);
}

static inline void printColor(const char *buffer, ColorOpt color) {
    std::cout << "\e[" << static_cast<int>(color.value_or(defaultCompletionColor)) << 'm' << buffer << "\e[0m";
}

void printCompletion(HistorySuggestion &history, int offset) {
    std::string pattern(lineBuffer.data());
    StringOpt completion;
    if (offset)
        completion = history.findCompletion(pattern);
    else
        completion = history.findNextCompletion(pattern);
    if (!completion) {
        return;
    }
    if (pattern.length() == completion.value().length()) {
        return;
    }
    clearTerminalLine();
    if (offset)
        cursor_forward(offset);
    printColor(completion.value().c_str() + pattern.length(), completionColor);
    cursor_backward(completion.value().length() - pattern.length() + offset);
    std::cout << std::flush;
}

CharOpt newlineHandler(HistorySuggestion &, Char) {
    lineBuffer.fill(0);
    lineBufferPos = lineBuffer.begin();
    return {};
}

CharOpt backspaceHandler(HistorySuggestion &, Char) {
    if (lineBufferPos != lineBuffer.begin()) {
        *(--lineBufferPos) = 0;
    }
    return {};
}

CharOpt regularCharHandler(HistorySuggestion &history, Char c) {
    *lineBufferPos = c;
    lineBufferPos++;
    printCompletion(history, 1);
    return {};
}

CharOpt tabHandler(HistorySuggestion &history, Char) {
    printCompletion(history, 0);
    return Char{0}; // TODO: this does not seem to work.
}

CharOpt arrowHandler2(HistorySuggestion &history, Char c) {
    if (arrowIndicator == 1) {
        arrowIndicator = 2;
        return {};
    }
    else {
        return regularCharHandler(history, c);
    }
}

CharOpt arrowHandler3(HistorySuggestion &history, Char c) {
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
        return_value = regularCharHandler(history, c);
    }
    return return_value;
}

namespace yb {

unsigned char yebash(HistorySuggestion &history, unsigned char c) {
    // TODO: uncomment later
    //if (!getenv("YEBASH"))
    //    return;
    auto handler = handlers[c];
    CharOpt cReturned;
    if (handler) {
        cReturned = handler(history, c);
    }
    else {
        if (c < 0x20) {
            newlineHandler(history, c);
        }
        else {
            regularCharHandler(history, c);
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
        *static_cast<unsigned char *>(buf) = yb::yebash(*historySuggestion, *static_cast<unsigned char *>(buf));
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
}

