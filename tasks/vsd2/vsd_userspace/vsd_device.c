#include <sys/mman.h>
#include <fcntl.h>

#include "vsd_device.h"
#include "../vsd_driver/vsd_ioctl.h"

#define VSD_FILENAME "/dev/vsd"


static int vsd_fd;

int vsd_init()
{
    vsd_fd = open(VSD_FILENAME, O_RDWR);
    return vsd_fd < 0 ? -1 : 0;
}

int vsd_deinit()
{
    return close(vsd_fd);
}

int vsd_get_size(size_t *out_size)
{
    struct vsd_ioctl_get_size_arg arg;

    if (ioctl(vsd_fd, VSD_IOCTL_GET_SIZE, &arg)) {
        return -1;
    }
    *out_size = arg.size;
    return 0;
}

int vsd_set_size(size_t size)
{
    struct vsd_ioctl_set_size_arg arg = { .size = size };

    if (ioctl(vsd_fd, VSD_IOCTL_SET_SIZE, &arg)) {
        return -1;
    }
    return 0;
}

ssize_t vsd_read(char* dst, off_t offset, size_t size)
{
    if (lseek(vsd_fd, offset, SEEK_SET) < 0) {
        return -1;
    }
    return read(vsd_fd, dst, size);
}

ssize_t vsd_write(const char* src, off_t offset, size_t size)
{
    if (lseek(vsd_fd, offset, SEEK_SET) < 0) {
        return -1;
    }
    return write(vsd_fd, src, size);
}

void* vsd_mmap(size_t offset)
{
    size_t size;

    if (vsd_get_size(&size) || offset >= size) {
        return MAP_FAILED;
    }
    return mmap(NULL, size - offset, PROT_READ | PROT_WRITE, MAP_SHARED, vsd_fd, offset);
}

int vsd_munmap(void* addr, size_t offset)
{
    size_t size;

    if (vsd_get_size(&size) || offset >= size) {
        return -1;
    }
    return munmap(addr, size - offset);
}
