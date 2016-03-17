#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "../vsd_driver/vsd_ioctl.h"

#define VSD_FILENAME "/dev/vsd"


void print_usage() {
    puts("Usage:\n"
        "vsd_userspace size_get\n"
        "vsd_userspace size_set SIZE_IN_BYTES");
}

int print_size() {
    int f;
    vsd_ioctl_get_size_arg_t arg;

    f = open(VSD_FILENAME, O_RDONLY | O_NONBLOCK);
    if (f < 0) {
        fprintf(stderr, "Can't open %s\n", VSD_FILENAME);
        return EXIT_FAILURE;
    }
    if (ioctl(f, VSD_IOCTL_GET_SIZE, &arg)) {
        fprintf(stderr, "Ioctl error: %d\n", errno);
        close(f);
        return EXIT_FAILURE;
    }
    close(f);
    printf("%lu\n", arg.size);
    return EXIT_SUCCESS;
}

int set_size(size_t size) {
    int f;
    vsd_ioctl_set_size_arg_t arg = { .size = size };

    f = open(VSD_FILENAME, O_RDONLY | O_NONBLOCK);
    if (f < 0) {
        fprintf(stderr, "Can't open %s\n", VSD_FILENAME);
        return EXIT_FAILURE;
    }
    if (ioctl(f, VSD_IOCTL_SET_SIZE, &arg)) {
        fprintf(stderr, "Ioctl error: %d\n", errno);
        close(f);
        return EXIT_FAILURE;
    }
    close(f);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage();
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "size_get") == 0) {
        return print_size();
    } else if (strcmp(argv[1], "size_set") == 0 && argc >= 3) {
        int size = atoi(argv[2]);
        if (size <= 0) {
            print_usage();
            return EXIT_FAILURE;
        }
        return set_size(size);
    } else {
        print_usage();
        return EXIT_FAILURE;
    }
}
