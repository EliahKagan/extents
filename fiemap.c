// ioctl-1.c - Trying out FIEMAP ioctl. See:
//  - https://lwn.net/Articles/260803/ and
//  - https://github.com/torvalds/linux/blob/master/include/uapi/linux/fiemap.h

#define _POSIX_C_SOURCE 1

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <linux/fiemap.h>
#include <linux/fs.h>
#include <sys/ioctl.h>

#ifdef SKIP_UNWRITTEN
enum { k_skip_unwritten = 1 };
#else
enum { k_skip_unwritten = 0 };
#endif

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

static __u32 min(const __u32 first, const __u32 second)
{
    return second < first ? second : first;
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

    return fm.fm_extent_count;
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
    struct fiemap *const fmp = get_fiemap(fileno(fp));
    const __u32 count = min(fmp->fm_extent_count, fmp->fm_mapped_extents);

    for (__u32 i = 0u; i < count; ++i) {
        const struct fiemap_extent *const fep = &fmp->fm_extents[i];

        if (k_skip_unwritten != (0)
                && (fep->fe_flags & FIEMAP_EXTENT_UNWRITTEN))
            continue;

        printf("physical: %13llu      logical: %9llu      length: %9llu\n",
                fep->fe_physical / 512, fep->fe_logical, fep->fe_length / 512);

        if (fep->fe_flags & FIEMAP_EXTENT_LAST) break;
    }

    free(fmp);
}

int main(int argc, char **argv)
{
    g_progname = argv[0];
    if (argc < 2) die("too few arguments");
    if (argc > 2) die("too many arguments");

    FILE *const fp = fopen(argv[1], "r");
    if (!fp) die("can't open \"%s\": %s", argv[1], strerror(errno));
    show_extents(fp);
    fclose(fp);

    return EXIT_SUCCESS;
}
