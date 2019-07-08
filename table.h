// table.h - drawing a custom table of file extents

#ifndef HAVE_IOCTL_1_FIEMAP_TABLE_H_
#define HAVE_IOCTL_1_FIEMAP_TABLE_H_

#include "feature-test.h"

#include "attribute.h"

#include <stddef.h>
#include <linux/fiemap.h>
#include <linux/types.h>
#include <sys/ioctl.h>

enum datum {
    datum_logical,      // "LOGICAL"
    datum_physical,     // "INITIAL"
    datum_length,       // "COUNT"
    datum_physical_end  // "FINAL"
};

struct colspec {
    const char *label;
    int width;
    enum datum datum;
    __u64 offset;
    __u64 divisor;
};

struct tablespec {
    const struct fiemap *fmp;
    int gap_width;
    int col_count;
    struct colspec cols[];
};

ATTRIBUTE((nonnull))
void show_extent_table(const struct fiemap *fmp, const __u64 offset,
                       const char *columns);

#endif // ! HAVE_IOCTL_1_FIEMAP_TABLE_H_
