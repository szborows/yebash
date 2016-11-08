#include <memory>
#include <dlfcn.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "yebash.hpp"
#include "Defs.hpp"
#include "HistorySuggestion.hpp"
#include "Printer.hpp"
#include "ArrowHandler.hpp"
#include "LineBuffer.hpp"
#include "EscapeCodeGenerator.hpp"

using namespace yb;

static thread_local History gHistory;
static thread_local std::unique_ptr<HistorySuggestion> historySuggestion = nullptr;
static thread_local std::unique_ptr<EscapeCodeGenerator> escapeCodeGenerator = nullptr;
static thread_local std::unique_ptr<Printer> printer = nullptr;
static thread_local std::unique_ptr<LineBuffer> lineBuffer = nullptr;

// TODO: these vars should also be static
thread_local std::unique_ptr<ArrowHandler> arrowHandler = nullptr;
thread_local std::string printBuffer;
thread_local std::string::iterator printBufferPos;

using ReadSignature = ssize_t (*)(int, void*, size_t);
static thread_local ReadSignature realRead = nullptr;

static constexpr const size_t defaultLineBufferSize = 1024;

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

static inline void loadHistory() {
    std::ifstream historyFile(std::string{getenv("HOME")} + "/.bash_history");
    if (!historyFile.is_open()) {
        throw std::runtime_error{"Could not open history file"};
    }
    gHistory.read(historyFile);
    historyFile.close();
}

static inline void createGlobals() {
    historySuggestion = std::make_unique<HistorySuggestion>(gHistory);
    escapeCodeGenerator = std::make_unique<ANSIEscapeCodeGenerator>();
    printer = std::make_unique<Printer>(std::cout, *escapeCodeGenerator);
    arrowHandler = std::make_unique<ArrowHandler>(*escapeCodeGenerator);
    lineBuffer = std::make_unique<LineBuffer>(defaultLineBufferSize);
}

__attribute__((constructor))
static void yebashInit()  {
    realRead = reinterpret_cast<ReadSignature>(dlsym(RTLD_NEXT, "read"));
    loadHistory();
    createGlobals();
}

