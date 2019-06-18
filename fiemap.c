// fiemap.c - Trying out FIEMAP ioctl. See:
//  - https://lwn.net/Articles/260803/ - documents FIEMAP as it was planned
//  - https://lwn.net/Articles/287905/ - documents some important changes
//  - https://github.com/torvalds/linux/blob/master/include/uapi/linux/fiemap.h

#include "feature-test.h"

#include "attribute.h"
#include "util.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/fiemap.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

enum filesystem_constants { k_sector_size = 512 };

// Prints major and minor device numbers and where the device seeems to start.
// Returns where the device seems to start.
ATTRIBUTE((nonnull))
static __u64 get_offset(const int fd)
{
    struct stat st = { 0 };
    if (fstat(fd, &st) != 0) die("can't stat: %s", strerror(errno));
    const unsigned maj = major(st.st_dev), min = minor(st.st_dev);

    enum { bufsz = 1024 };
    char path[bufsz] = { 0 };
    if (snprintf(path, bufsz, "/sys/dev/block/%u:%u/start", maj, min) >= bufsz)
        die("internal error: sysfs path exceeds buffer");

    FILE *const sysfp = fopen(path, "r");
    if (!sysfp) die("%s: %s", path, strerror(errno));

    __u64 offset_in_sectors = 0uLL;
    char extra = '\0';
    if (fscanf(sysfp, "%llu %c", &offset_in_sectors, &extra) != 1)
        die("can't interpret %s as offset", path);

    const __u64 offset = offset_in_sectors * k_sector_size;
    printf("On block device %u:%u starting at %llu (sector %llu):\n",
            maj, min, offset, offset_in_sectors);

    return offset;
}

ATTRIBUTE((malloc, returns_nonnull))
static struct fiemap *alloc_fiemap(const __u32 extent_count)
{
    return xcalloc(1u, sizeof(struct fiemap)
                        + sizeof(struct fiemap_extent) * (size_t)extent_count);
}

static __u32 count_extents(const int fd)
{
    struct fiemap fm = { .fm_length = ULLONG_MAX, .fm_extent_count = 0 };

    if (ioctl(fd, FS_IOC_FIEMAP, &fm) != 0)
        die("can't count extents: %s", strerror(errno));

    return fm.fm_mapped_extents;
}

// The number of extents may change between when they are counted and when the
// count is used. This is a TOCTOU (time of check/time of use) race condition.
// This checks that they still fit, and quits with an error if they don't.
ATTRIBUTE((nonnull))
static void ensure_extents_retrieved(const struct fiemap *const fmp)
{
    assert(fmp);
    assert(fmp->fm_mapped_extents <= fmp->fm_extent_count);

    if (fmp->fm_mapped_extents
            && !(fmp->fm_extents[fmp->fm_mapped_extents - 1u].fe_flags
                 & FIEMAP_EXTENT_LAST))
        die("extent count just increased!");
}

ATTRIBUTE((returns_nonnull))
static struct fiemap *get_fiemap(const int fd)
{
    const __u32 extent_count = count_extents(fd);
    struct fiemap *const fmp = alloc_fiemap(extent_count);

    fmp->fm_start = 0uLL;
    fmp->fm_length = ULLONG_MAX;
    fmp->fm_extent_count = extent_count;
    fmp->fm_flags = 0u;

    if (ioctl(fd, FS_IOC_FIEMAP, fmp) != 0)
        die("can't retrieve extents: %s", strerror(errno));

    assert(fmp->fm_extent_count == extent_count); // See fiemap.h.
    ensure_extents_retrieved(fmp);
    return fmp;
}

enum format_widths {
    k_logical_width = 11, k_logical_short_width = 8,
    k_physical_width = 13, k_physical_short_width = 10,
    k_length_width = 9, k_length_short_width = 6
};

static void show_labels(void)
{
    printf("%*s     %*s   %*s     %*s   %*s\n",
            k_logical_width, "LOGICAL", k_logical_short_width, "",
            k_physical_width, "PHYSICAL", k_physical_short_width, "",
            k_length_width, "LENGTH");
}

ATTRIBUTE((nonnull(1)))
static void show_extent(const struct fiemap_extent *const fep,
                        const __u64 offset)
{
    assert(fep);
    const __u64 physical = offset + fep->fe_physical;

    printf("%*llu B = %*llu   %*llu B = %*llu   %*llu B = %*llu\n",
            k_logical_width, fep->fe_logical,
            k_logical_short_width, fep->fe_logical / k_sector_size,
            k_physical_width, physical,
            k_physical_short_width, physical / k_sector_size,
            k_length_width, fep->fe_length,
            k_length_short_width, fep->fe_length / k_sector_size);
}

ATTRIBUTE((nonnull))
static void show_all_extents(const int fd)
{
    const __u64 offset = get_offset(fd);
    struct fiemap *const fmp = get_fiemap(fd);

    show_labels();
    for (__u32 i = 0u; i < fmp->fm_mapped_extents; ++i)
        show_extent(&fmp->fm_extents[i], offset);

    free(fmp);
}

int main(int argc, char **argv)
{
    set_progname(argv[0]);
    if (argc < 2) die("too few arguments");
    if (argc > 2) die("too many arguments");

    FILE *const fp = fopen(argv[1], "rb");
    if (!fp) die("%s: %s", argv[1], strerror(errno));
    show_all_extents(fileno(fp));
    fclose(fp);

    return EXIT_SUCCESS;
}
