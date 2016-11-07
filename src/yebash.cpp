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
#include "Printer.hpp"
#include "ArrowHandler.hpp"
#include "LineBuffer.hpp"
#include "KeyHandlers.hpp"

// https://www.akkadia.org/drepper/tls.pdf
// http://www.tldp.org/HOWTO/Bash-Prompt-HOWTO/x361.html
// http://www.linusakesson.net/programming/tty/

using namespace yb;

thread_local std::string printBuffer;
thread_local std::string::iterator printBufferPos;

thread_local History gHistory;

using ReadSignature = ssize_t (*)(int, void*, size_t);
static thread_local ReadSignature realRead = nullptr;

constexpr const Color defaultSuggestionColor = Color::grey;
thread_local ColorOpt suggestionColor = {};

thread_local std::unique_ptr<HistorySuggestion> historySuggestion = nullptr;
thread_local std::unique_ptr<EscapeCodeGenerator> escapeCodeGenerator = nullptr;
thread_local std::unique_ptr<Printer> printer = nullptr;
thread_local std::unique_ptr<ArrowHandler> arrowHandler = nullptr;
thread_local std::unique_ptr<LineBuffer> lineBuffer = nullptr;

thread_local std::unordered_map<Char, std::function<CharOpt(HistorySuggestion &, Printer &, LineBuffer &, Char)>> handlers = {
    {0x06, tabHandler},
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

CharOpt tabHandler(HistorySuggestion &history, Printer &printer, LineBuffer &buffer, Char) {
    printSuggestion(history, printer, buffer, 0);
    return Char{0}; // TODO: this does not seem to work.
}

unsigned char yebash(HistorySuggestion &history, Printer &printer, LineBuffer &buffer, unsigned char c) {
    // TODO: uncomment later
    //if (!getenv("YEBASH"))
    //    return;
    // TODO: arrowHandler should be a parameter
    auto arrow = arrowHandler->handle(c);
    if (arrow) {
        switch (arrow.value()) {
            case Arrow::right:
                printBuffer = historySuggestion->get().substr(buffer.getPosition());
                printBufferPos = printBuffer.begin();
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

static inline bool is_terminal_input(int fd) {
    return isatty(fd) && fd == 0;
}

static inline void putCharToReadBuffer(char *buf) {
    *buf = *printBufferPos;
    lineBuffer->insert(*printBufferPos++);
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
        *static_cast<unsigned char *>(buf) = yb::yebash(*historySuggestion, *printer, *lineBuffer, *static_cast<unsigned char *>(buf));
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
    arrowHandler = std::make_unique<ArrowHandler>(*escapeCodeGenerator);
    lineBuffer = std::make_unique<LineBuffer>();
}

