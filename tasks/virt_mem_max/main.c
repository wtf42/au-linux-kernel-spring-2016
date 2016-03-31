#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

#define KB ((size_t)1024)
#define MB (KB * 1024)
#define GB (MB * 1024)
#define TB (GB * 1024)

int try_allocate(size_t size) {
    void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_NORESERVE | MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (mem == MAP_FAILED) {
        return 0;
    } else {
        if (munmap(mem, size)) {
            perror("failed to unmap");
            exit(1);
        }
        return 1;
    }
}

int main()
{
    int page_size = getpagesize();
    size_t max_size = page_size;
    while (try_allocate(max_size))
        max_size *= 2;
    size_t l = 0, r = max_size;
    while (l < r) {
        size_t m = (l + r) / 2;
        if (try_allocate(m)) {
            l = m + 1;
        } else {
            r = m;
        }
    }
    max_size = l;
    printf("%zu B, %zu KB, %zu MB, %zu GB, %zu TB\n",
        max_size,
        max_size / KB,
        max_size / MB,
        max_size / GB,
        max_size / TB);
    return 0;
}
