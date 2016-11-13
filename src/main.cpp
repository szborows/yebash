#include <memory>
#include <dlfcn.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <boost/property_tree/ini_parser.hpp>
#include <sys/types.h>
#include <pwd.h>

#include "yebash.hpp"
#include "Defs.hpp"
#include "HistorySuggestion.hpp"
#include "Printer.hpp"
#include "ArrowHandler.hpp"
#include "LineBuffer.hpp"
#include "EscapeCodeGenerator.hpp"
#include "PrintBuffer.hpp"

using namespace yb;

static constexpr const size_t defaultLineBufferSize = 1024;
static constexpr const size_t defaultPrintBufferSize = 1024;

static thread_local History history;
static thread_local std::unique_ptr<HistorySuggestion> historySuggestion = nullptr;
static thread_local std::unique_ptr<EscapeCodeGenerator> escapeCodeGenerator = nullptr;
static thread_local std::unique_ptr<Printer> printer = nullptr;
static thread_local std::unique_ptr<ArrowHandler> arrowHandler = nullptr;
static thread_local LineBuffer lineBuffer(defaultLineBufferSize);
static thread_local PrintBuffer printBuffer(defaultPrintBufferSize);

using ReadSignature = ssize_t (*)(int, void*, size_t);
static thread_local ReadSignature realRead = nullptr;

static inline bool is_terminal_input(int fd) {
    return isatty(fd) && fd == 0;
}

static inline void putCharToReadBuffer(char *buf) {
    auto c = printBuffer.getNextChar();
    if (c) {
        *buf = c.value();
        lineBuffer.insert(c.value());
    }
}

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

static inline void loadHistory() {
    std::ifstream historyFile(std::string{getenv("HOME")} + "/.bash_history");
    if (!historyFile.is_open()) {
        throw std::runtime_error{"Could not open history file"};
    }
    history.read(historyFile);
    historyFile.close();
}

static inline void createGlobals() {
    historySuggestion = std::make_unique<HistorySuggestion>(history);
    escapeCodeGenerator = std::make_unique<ANSIEscapeCodeGenerator>();
    printer = std::make_unique<Printer>(std::cout, *escapeCodeGenerator);
    arrowHandler = std::make_unique<ArrowHandler>(*escapeCodeGenerator);
}

static auto readConfiguration() {
    passwd *pw = getpwuid(getuid());
    std::string const configFile = std::string{pw->pw_dir} + "/.yebash.cfg";

    namespace pt = boost::property_tree;
    try {
        pt::ptree tree;
        pt::ini_parser::read_ini(configFile, tree);
        tree.get<std::string>("color.completion");
    }
    catch(pt::ini_parser::ini_parser_error const&) {
        return; // config file doesn't exist
    }
    catch (pt::ptree_bad_path const&) {
        return; // no color.completion in config file
    }
}

__attribute__((constructor))
static void yebashInit()  {
    realRead = reinterpret_cast<ReadSignature>(dlsym(RTLD_NEXT, "read"));
    readConfiguration();
    loadHistory();
    createGlobals();
}

