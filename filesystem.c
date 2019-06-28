// filesystem.c - implementation of general filesystem access functions

#include "filesystem.h"
#include "util.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <linux/types.h>
#include <sys/sysmacros.h>
#include <sys/types.h>

FILE *open_file(const char *const path)
{
    assert(path);

    FILE *const fp = fopen(path, "rb");
    if (!fp) die("%s: %s", path, strerror(errno));
    return fp;
}

__u64 get_offset(const dev_t dev)
{
    const unsigned maj = major(dev), min = minor(dev);

    enum { bufsz = 1024 };
    char path[bufsz] = { 0 };
    if (snprintf(path, bufsz, "/sys/dev/block/%u:%u/start", maj, min) >= bufsz)
        die("sysfs path exceeds buffer (this is a bug!)");

    FILE *const sysfp = fopen(path, "r");
    if (!sysfp) die("%s: %s", path, strerror(errno));

    __u64 offset_in_sectors = 0uLL;
    char extra = '\0';
    if (fscanf(sysfp, "%llu %c", &offset_in_sectors, &extra) != 1)
        die("can't interpret %s as offset", path);

    const __u64 offset = offset_in_sectors * k_sector_size;
    printf("On block device %u:%u, which starts at byte %llu (sector %llu):\n",
            maj, min, offset, offset_in_sectors);

    return offset;
}
