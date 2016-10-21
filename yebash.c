#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static ssize_t (*real_read)(int fd, void *buf, size_t count) = NULL;

static char line[256];
char to_print[256];
static size_t to_print_size = 0;
static size_t to_print_index = 0;

static int i = 0;
FILE *history_file = NULL;

#define cursor_forward(x) printf("\033[%dC", (x))
#define cursor_backward(x) printf("\033[%dD", (x))

char arrow = 0;

int open_history_file(FILE **f) {

    char history[256], *home_dir = getenv("HOME");

    strcpy(history, home_dir);
    strcat(history, "/.bash_history");

    if ((*f = fopen(history, "r")) == NULL) {
        printf("No such file %s\n", history);
        return -1;
    }

    return 0;

}

void complete_from_history(char *pattern, char *result) {

    char *buffer;
    size_t len;

    if (!history_file) return;

    while (getline(&buffer, &len, history_file) != -1) {
        *strchr(buffer, '\n') = 0;
        if (!strncmp(buffer, line, i)) {
            strcpy(result, buffer + strlen(pattern));
            break;
        }
        free(buffer);
    }
}

void interactive_completion(char c) {

    size_t len;
    char *buffer = 0;

    if (!history_file)
        if (open_history_file(&history_file)) return;

    switch (c) {
        case 0xd: // newline
            arrow = 0;
            i = 0;
            memset(line, 0, 256);
            break;
        case 0x1b: // first char of arrow key
            arrow = 1;
            break;
        case 0x5b: // second
            if (arrow == 1) arrow = 2;
            break;
        case 0x43: // third
            if (arrow == 2) {
                arrow = 0;
                while (getline(&buffer, &len, history_file) != -1) {
                    *strchr(buffer, '\n') = 0;
                    if (!strncmp(buffer, line, i)) {
                        strcpy(to_print, buffer + i);
                        to_print_size = strlen(buffer) - i;
                        to_print_index = 0;
                    }
                }
            }
            break;
        default:
            arrow = 0;
            line[i++] = c;

            while (getline(&buffer, &len, history_file) != -1) {
                *strchr(buffer, '\n') = 0;
                if (!strncmp(buffer, line, i)) {
                    if (strlen(buffer) > strlen(line)) {
                        cursor_forward(1);
                        printf("\e[1;30m%s\e[0m", buffer + i);
                        cursor_backward((int)strlen(buffer) - i + 1);
                    } else {
                        //printf("          ");
                        //cursor_backward(10);
                    }
                    fflush(stdout);
                    break;
                }
            }
    }

}



ssize_t read(int fd, void *buf, size_t count) {

    ssize_t ret;

    if (fd == 0) {
        if (to_print_size) {
            *(char *)buf = to_print[to_print_index++];
            if (to_print_index == to_print_size) {
                to_print_size = 0;
                to_print_index = 0;
            }
            return 1;
        }
    }

    if (!real_read)
        real_read = dlsym(RTLD_NEXT, "read");
    ret = real_read(fd, buf, count);

    if (fd == 0) {
        interactive_completion(*(char *)buf);
        if (to_print_size) {
            *(char *)buf = to_print[to_print_index++];
            if (to_print_index == to_print_size) {
                to_print_size = 0;
                to_print_index = 0;
            }
            return 1;
        }

    }

    return ret;

}


