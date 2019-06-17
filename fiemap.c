// ioctl-1.c - Trying out FIEMAP ioctl. See:
//  - https://lwn.net/Articles/260803/ and
//  - https://github.com/torvalds/linux/blob/master/include/uapi/linux/fiemap.h

#include "feature-test.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <linux/fiemap.h>
#include <linux/fs.h>
#include <sys/ioctl.h>

enum constants { k_sector_size = 512 };

static const char *g_progname;

static noreturn void die(const char *const format, ...)
{
    fprintf(stderr, "%s: error: ", g_progname);

    va_list vlist;
    va_start(vlist, format);
    vfprintf(stderr, format, vlist);
    va_end(vlist);

    putc('\n', stderr);

    exit(EXIT_FAILURE);
}

static void *xcalloc(const size_t count, const size_t size)
{
    void *const ret = calloc(count, size);
    if (!ret) die("out of memory");
    return ret;
}

static struct fiemap *alloc_fiemap(const __u32 extent_count)
{
    return xcalloc(1u, sizeof(struct fiemap)
                        + sizeof(struct fiemap_extent) * (size_t)extent_count);
}

static __u32 count_extents(const int fd)
{
    struct fiemap fm = { .fm_length = ~0ULL, .fm_extent_count = 0 };

    if (ioctl(fd, FS_IOC_FIEMAP, &fm) != 0)
        die("ioctl error counting extents: %s", strerror(errno)); // TODO: does it set errno?

    return fm.fm_mapped_extents;
}

static struct fiemap *get_fiemap(const int fd)
{
    const __u32 extent_count = count_extents(fd);
    struct fiemap *const fmp = alloc_fiemap(extent_count);

    fmp->fm_start = 0;
    fmp->fm_length = ~0ULL;
    fmp->fm_extent_count = extent_count;
    fmp->fm_flags = 0;

    if (ioctl(fd, FS_IOC_FIEMAP, fmp) != 0)
        die("ioctl error retrieving extents: %s", strerror(errno)); // TODO: does it set errno?

    // Protect against UB due to TOCTOU race condition.
    if (fmp->fm_mapped_extents != extent_count)
        die("number of extents changed unexpectedly");

    return fmp;
}

static void show_extents(FILE *const fp)
{
    enum {
        logical_width = 11, logical_short_width = 8,
        physical_width = 13, physical_short_width = 10,
        length_width = 9, length_short_width = 6
    };

    struct fiemap *const fmp = get_fiemap(fileno(fp));

    printf("%*s     %*s   %*s     %*s   %*s\n",
            logical_width, "LOGICAL", logical_short_width, "",
            physical_width, "PHYSICAL", physical_short_width, "",
            length_width, "LENGTH");

    for (__u32 i = 0u; i < fmp->fm_extent_count; ++i) {
        const struct fiemap_extent *const fep = &fmp->fm_extents[i];

        printf("%*llu B = %*llu   %*llu B = %*llu   %*llu B = %*llu\n",
                logical_width, fep->fe_logical,
                logical_short_width, fep->fe_logical / k_sector_size,
                physical_width, fep->fe_physical,
                physical_short_width, fep->fe_physical / k_sector_size,
                length_width, fep->fe_length,
                length_short_width, fep->fe_length / k_sector_size);
    }

    free(fmp);
}

int main(int argc, char **argv)
{
    g_progname = argv[0];
    if (argc < 2) die("too few arguments");
    if (argc > 2) die("too many arguments");

    FILE *const fp = fopen(argv[1], "rb");
    if (!fp) die("can't open \"%s\": %s", argv[1], strerror(errno));
    show_extents(fp);
    fclose(fp);

    return EXIT_SUCCESS;
}
