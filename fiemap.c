/* ioctl-1.c - Trying out FIEMAP ioctl.
 *     See https://lwn.net/Articles/260803/.
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
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

static void die(const char *const format, ...)
{
    fprintf(stderr, "%s: error: ", g_progname);

    {
        va_list vlist;
        va_start(vlist, format);
        vfprintf(stderr, format, vlist);
        va_end(vlist);
    }

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
    return xcalloc(1, sizeof(struct fiemap)
                        + sizeof(struct fiemap_extent) * (size_t)extent_count);
}

static void show_extents(const int fd)
{
    /* TODO: Get extent_count dynamically via an initial fiemap ioctl call with
             FIEMAP_FLAG_NUM_EXTENTS. But keep race conditions in mind! */
    enum { extent_count = 1024 };

    struct fiemap *const fmp = alloc_fiemap(extent_count);
    int i = 0;

    fmp->fm_start = 0;
    fmp->fm_length = ~0ULL;
    fmp->fm_extent_count = extent_count;
    fmp->fm_flags = 0; /* not sure if this would work */

    if (ioctl(fd, FS_IOC_FIEMAP, fmp) != 0)
        die("ioctl error: %s", strerror(errno)); /* TODO: does it set errno? */

    for (i = 0; i < extent_count; ++i) {
        const struct fiemap_extent *const fep = &fmp->fm_extents[i];

        if (k_skip_unwritten && (fep->fe_flags & FIEMAP_EXTENT_UNWRITTEN))
            continue;

        printf("physical: %13llu      logical: %9llu      length: %9llu\n",
                fep->fe_physical / 512, fep->fe_logical, fep->fe_length / 512);

        if (fep->fe_flags & FIEMAP_EXTENT_LAST) break;
    }

    free(fmp);
}

int main(int argc, char **argv)
{
    FILE *fp = NULL;

    g_progname = argv[0];
    if (argc < 2) die("too few arguments");
    if (argc > 2) die("too many arguments");

    fp = fopen(argv[1], "r");
    if (!fp) die("can't open \"%s\": %s", argv[1], strerror(errno));
    show_extents(fileno(fp));
    fclose(fp);

    return EXIT_SUCCESS;
}
