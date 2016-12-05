#include <memory>
#include <dlfcn.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sys/select.h>

#include "yebash.hpp"
#include "Defs.hpp"
#include "HistorySuggestion.hpp"
#include "Printer.hpp"
#include "ArrowHandler.hpp"
#include "LineBuffer.hpp"
#include "EscapeCodeGenerator.hpp"
#include "PrintBuffer.hpp"

using namespace yb;

constexpr const size_t defaultLineBufferSize = 1024;
constexpr const size_t defaultPrintBufferSize = 1024;

namespace {

thread_local History history;
thread_local std::unique_ptr<HistorySuggestion> historySuggestion = nullptr;
thread_local std::unique_ptr<EscapeCodeGenerator> escapeCodeGenerator = nullptr;
thread_local std::unique_ptr<Printer> printer = nullptr;
thread_local std::unique_ptr<ArrowHandler> arrowHandler = nullptr;
thread_local LineBuffer lineBuffer(defaultLineBufferSize);
thread_local PrintBuffer printBuffer(defaultPrintBufferSize);

using ReadSignature = ssize_t (*)(int, void*, size_t);
thread_local ReadSignature realRead = nullptr;
using PSelectSignature = int (*)(int, fd_set *, fd_set *, fd_set *, const struct timespec *, const sigset_t *);
thread_local PSelectSignature realPSelect = nullptr;

bool is_terminal_input(int fd) {
    return isatty(fd) && fd == 0;
}

void putCharToReadBuffer(char *buf) {
    auto c = printBuffer.getNextChar();
    if (c) {
        *buf = c.value();
        lineBuffer.insert(c.value());
    }
}

} // namespace anon

ssize_t read(int fd, void *buf, size_t count) {
    if (is_terminal_input(fd) && !printBuffer.empty()) {
        putCharToReadBuffer(static_cast<char *>(buf));
        return 1;
    }
    auto returnValue = realRead(fd, buf, count);
    if (is_terminal_input(fd)) {
        *static_cast<unsigned char *>(buf) = yb::yebash(*historySuggestion, *printer, lineBuffer, printBuffer, *arrowHandler, *static_cast<unsigned char *>(buf));
    }
    return returnValue;
}

int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *timeout, const sigset_t *sigmask) {
    int returnValue;
    returnValue = realPSelect(nfds, readfds, writefds, exceptfds, timeout, sigmask);
    return returnValue;
}

void loadHistory() {
    std::ifstream historyFile(std::string{getenv("HOME")} + "/.bash_history");
    if (!historyFile.is_open()) {
        throw std::runtime_error{"Could not open history file"};
    }
    history.read(historyFile);
    historyFile.close();
}

void createGlobals() {
    historySuggestion = std::make_unique<HistorySuggestion>(history);
    escapeCodeGenerator = std::make_unique<ANSIEscapeCodeGenerator>();
    printer = std::make_unique<Printer>(std::cout, *escapeCodeGenerator);
    arrowHandler = std::make_unique<ArrowHandler>(*escapeCodeGenerator);
}

__attribute__((constructor))
void yebashInit()  {
    realRead = reinterpret_cast<ReadSignature>(dlsym(RTLD_NEXT, "read"));
    realPSelect = reinterpret_cast<PSelectSignature>(dlsym(RTLD_NEXT, "pselect"));
    loadHistory();
    createGlobals();
}

