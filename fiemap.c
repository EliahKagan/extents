// fiemap.c - Trying out FIEMAP ioctl. See:
//  - https://lwn.net/Articles/260803/ - documents FIEMAP as it was planned
//  - https://lwn.net/Articles/287905/ - documents some important changes
//  - https://github.com/torvalds/linux/blob/master/include/uapi/linux/fiemap.h

#include "feature-test.h"

#include "attribute.h"
#include "constants.h"
#include "table.h"
#include "util.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/fiemap.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>

ATTRIBUTE((nonnull, returns_nonnull))
static FILE *open_file(const char *const path)
{
    assert(path);

    FILE *const fp = fopen(path, "rb");
    if (!fp) die("%s: %s", path, strerror(errno));
    return fp;
}

// Prints major and minor device numbers and where the device seeems to start.
// Returns where the device seems to start.
static __u64 get_offset(const dev_t dev)
{
    const unsigned maj = major(dev), min = minor(dev);

    enum { bufsz = 1024 };
    char path[bufsz] = {0};
    if (snprintf(path, bufsz, "/sys/dev/block/%u:%u/start", maj, min) >= bufsz)
        die(BUG("sysfs path exceeds buffer"));

    FILE *const sysfp = fopen(path, "r");
    if (!sysfp)
        die("%s: %s", path, strerror(errno));

    __u64 offset_in_sectors = 0uLL;
    char extra = '\0';
    if (fscanf(sysfp, "%llu %c", &offset_in_sectors, &extra) != 1)
        die("can't interpret %s as offset", path);

    const __u64 offset = offset_in_sectors * k_sector_size;
    printf("On block device %u:%u, "
            "which starts at byte %llu (sector %llu):\n\n",
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

ATTRIBUTE((nonnull, returns_nonnull))
static const struct fiemap_extent *last_extent(const struct fiemap *const fmp)
{
    assert(fmp);
    assert(fmp->fm_mapped_extents);
    return &fmp->fm_extents[fmp->fm_mapped_extents - 1u];
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
            && !(last_extent(fmp)->fe_flags & FIEMAP_EXTENT_LAST))
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

    assert(fmp->fm_extent_count == extent_count); // See <linux/fiemap.h>.
    ensure_extents_retrieved(fmp);
    return fmp;
}

// Currently this is just the length of the string, because the user specifies
// each column using a single character, with no extraneous characters allowed.
ATTRIBUTE((nonnull))
static int count_columns(const char *const columns)
{
    ASSERT_NONNEGATIVE_INT_FITS_IN_SIZE_T();
    assert(columns);

    const size_t count = strlen(columns);

    if (count > (size_t)INT_MAX)
        die("you want more than %d columns?", INT_MAX);

    return (int)count;
}

ATTRIBUTE((nonnull))
static void specify_column(struct colspec *const colp,
                           const __u64 offset, const char column)
{
    assert(colp);

    switch (column) {
    case 'l':
        colp->label = "LOGICAL (B)";
        colp->datum = datum_logical;
        colp->offset = 0uLL;
        colp->divisor = 1uLL;
        break;

    case 'L':
        colp->label = "LOGICAL (sec)";
        colp->datum = datum_logical;
        colp->offset = 0uLL;
        colp->divisor = k_sector_size;
        break;

    case 'i':
        colp->label = "INITIAL (B)";
        colp->datum = datum_physical;
        colp->offset = offset;
        colp->divisor = 1uLL;
        break;

    case 'I':
        colp->label = "INITIAL (sec)";
        colp->datum = datum_physical;
        colp->offset = offset;
        colp->divisor = k_sector_size;
        break;

    case 'f':
        colp->label = "FINAL (B)";
        colp->datum = datum_physical_end;
        colp->offset = offset - 1uLL;
        colp->divisor = 1uLL;
        break;

    case 'F':
        colp->label = "FINAL (sec)";
        colp->datum = datum_physical_end;
        colp->offset = offset - 1uLL;
        colp->divisor = k_sector_size;
        break;

    case 'c':
        colp->label = "COUNT (B)";
        colp->datum = datum_length;
        colp->offset = 0uLL;
        colp->divisor = 1uLL;
        break;

    case 'C':
        colp->label = "COUNT (sec)";
        colp->datum = datum_length;
        colp->offset = 0uLL;
        colp->divisor = k_sector_size;
        break;

    default:
        die("unrecognized column specifier \"%c\"", column);
    }
}

ATTRIBUTE((nonnull))
void show_extent_table(const struct fiemap *const fmp, const __u64 offset,
                       const char *const columns)
{
    enum { gap_width = 3 }; // TODO: Let the user customize this.
    assert(fmp);
    assert(columns);

    struct tablespec *const tsp = alloc_tablespec(count_columns(columns));
    tsp->fmp = fmp;
    tsp->gap_width = gap_width;

    for (int i = 0; i < tsp->col_count; ++i)
        specify_column(&tsp->cols[i], offset, columns[i]);

    populate_widths(tsp);
    show_table(tsp);
    free(tsp);
}

ATTRIBUTE((nonnull))
static __u64 sum_extents(const struct fiemap *const fmp)
{
    assert(fmp);

    __u64 sum = 0uLL;
    for (__u32 i = 0u; i < fmp->fm_mapped_extents; ++i)
        sum += fmp->fm_extents[i].fe_length;

    return sum;
}

ATTRIBUTE((nonnull(1)))
static void show_end(const struct fiemap *const fmp, const __u64 real_size)
{
    assert(fmp);
    assert(fmp->fm_mapped_extents);

    const __u64 sum = sum_extents(fmp);

    if (sum < real_size) {
        die("file is %llu bytes; extents only use %llu bytes",
                real_size, sum);
    }

    printf("%llu/%llu bytes used", real_size, sum);

    const __u64 unused = sum - real_size;
    const __u64 last_length = last_extent(fmp)->fe_length;

    if (last_length < unused) {
        puts("."); // Finish the last message before quitting with an error.

        die("unused space (%llu bytes) exceeds last extent (%llu bytes)",
                unused, last_length);
    }

    printf(", %llu/%llu in the last extent.\n",
            last_length - unused, last_length);
}

ATTRIBUTE((nonnull(1)))
static void show_interpretation_guide(const struct fiemap *const fmp,
                                      const off_t size)
{
    assert(fmp);

    putchar('\n');

    if (size < 0)
        die("file has negative size %lld", (long long)size);
    else if (fmp->fm_mapped_extents)
        show_end(fmp, (__u64)size);
    else if (size)
        die("size is %llu bytes but there are no extents", (long long)size);
    else
        puts("There are no extents.");
}

static void show_extent_info(const int fd)
{
    struct stat st = { 0 };
    if (fstat(fd, &st) != 0) die("can't stat: %s", strerror(errno));

    const __u64 offset = get_offset(st.st_dev);
    struct fiemap *const fmp = get_fiemap(fd);

    show_extent_table(fmp, offset, "LIFC");
    show_interpretation_guide(fmp, st.st_size);

    free(fmp);
}

int main(int argc, char **argv)
{
    set_progname(argv[0]);
    if (argc < 2) die("too few arguments");
    if (argc > 2) die("too many arguments");

    FILE *const fp = (strcmp(argv[1], "-") == 0 ? stdin : open_file(argv[1]));
    show_extent_info(fileno(fp));
    if (fp != stdin) fclose(fp);
}
