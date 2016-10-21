#include <dlfcn.h>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <sys/ioctl.h>
#include <unistd.h>
#include <array>

typedef ssize_t (*ReadSignature)(int, void*, size_t);

// TODO: per-process memory?
using LineBuffer = std::array<char, 1024>;
static LineBuffer line_buffer;
static auto line_buffer_pos = line_buffer.begin();

#define cursor_forward(x) printf("\033[%dC", (x))
#define cursor_backward(x) printf("\033[%dD", (x))

int open_history_file(FILE **f) {
    char history[512], *home_dir = getenv("HOME");

    strcpy(history, home_dir);
    strcat(history, "/.bash_history");

    if ((*f = fopen(history, "r")) == NULL) {
        printf("No such file %s\n", history);
        return -1;
    }

    return 0;
}

void complete_from_history(char *pattern, char *result) {

    char *buffer = 0;
    size_t len = 0;

    FILE *f = 0;

    open_history_file(&f);

    while (getline(&buffer, &len, f) != -1) {
        *strchr(buffer, '\n') = 0;
        if (!strncmp(buffer, pattern, strlen(pattern))) {
            strcpy(result, buffer + strlen(pattern));
            break;
        }
    }

    fclose(f);

}

void interactive_completion(unsigned char c) {

    char buffer[2048];
    struct winsize w;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); // TODO:

    switch (c) {
        case 0xd: // newline
            line_buffer_pos = line_buffer.begin();
            std::fill(std::begin(line_buffer), std::end(line_buffer), 0);
            memset(buffer, 0, 2048);
            break;

        case 0x7f: // backspace
            if (line_buffer_pos != line_buffer.begin()) {
                *(--line_buffer_pos) = 0;
            }
            break;

        default:
            *(++line_buffer_pos) = c;
            *(line_buffer_pos) = 0;
            complete_from_history(line_buffer.data(), buffer);
            if (strlen(buffer) > strlen(line_buffer.data())) {
                cursor_forward(1);
                printf("\e[1;30m%s\e[0m", buffer);
                cursor_backward((int)strlen(buffer) + 1);
                fflush(stdout);
            }
            break;
   }

}

ssize_t read(int fd, void *buf, size_t count) {
    static ReadSignature real_read = reinterpret_cast<ReadSignature>(dlsym(RTLD_NEXT, "read"));
    ssize_t ret = real_read(fd, buf, count);

    if (0 == fd) {
        interactive_completion(*(unsigned char *)buf);
    }

    return ret;
}

