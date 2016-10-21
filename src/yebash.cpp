#include <dlfcn.h>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <sys/ioctl.h>
#include <unistd.h>

typedef ssize_t (*ReadSignature)(int, void*, size_t);
static ReadSignature real_read = nullptr;

static char __line[1024];
static int __line_index = 0;

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
            __line_index = 0;
            memset(__line, 0, 1024);
            memset(buffer, 0, 2048);
            break;

        case 0x7f: // backspace
            if (__line_index > 0)
                __line[--__line_index] = 0;
            break;

        default:
            __line[__line_index++] = c;
            __line[__line_index] = 0;
            complete_from_history(__line, buffer);
            if (strlen(buffer) > strlen(__line)) {
                cursor_forward(1);
                printf("\e[1;30m%s\e[0m", buffer);
                cursor_backward((int)strlen(buffer) + 1);
                fflush(stdout);
            }
            break;
   }

}

ssize_t read(int fd, void *buf, size_t count) {

    ssize_t ret;

    if (!real_read)
        real_read = reinterpret_cast<ReadSignature>(dlsym(RTLD_NEXT, "read"));
    ret = real_read(fd, buf, count);

    if (fd == 0)
        interactive_completion(*(unsigned char *)buf);

    return ret;

}


